
#include <GL\glew.h>
#include "gearoenix-scene.hpp"
#include "gearoenix-ortho-camera.hpp"
#include "gearoenix-shader-engine.hpp"
#include "gearoenix-log.hpp"
#include "gearoenix-main.hpp"
#include <FreeImage.h>
#include <sstream>
#include <random>
#include "gearoenix-shortcuts.hpp"

static GLuint bo[4];
static GLuint shRenPrg;
static GLuint vtxAttInd;
static GLuint uvAttInd;
static GLuint mvpLoc;
static GLuint txtLoc;
static GLuint txtbuf[2];
static GLuint txtmapbuf;

gearoenix::render::Scene::Scene(const char *const &fileAddress, const unsigned int &mapAspect, const unsigned int &numIter)
{
	camera = new math::OrthoCamera(0.1f, 100.0f, float(ui::MainWindow::instance->winWidth) / float(ui::MainWindow::instance->winHeight));
	GLfloat meshW;
	{
		glGenTextures(2, txtbuf);
	}
	{
		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(fileAddress), fileAddress);
		FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
		int nWidth = FreeImage_GetWidth(pImage);
		int nHeight = FreeImage_GetHeight(pImage);
		meshW = GLfloat(nWidth) / GLfloat(nHeight);
		GLfloat *pixels = new GLfloat[nWidth * nHeight * 4];
		const int mapSize = mapAspect * mapAspect * 4;
		float *map = new float[mapSize];
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
		for (int i = 0; i < mapSize; ++i)
			map[i] = uniform_dist(e1);
		unsigned char *orig = (unsigned char *)FreeImage_GetBits(pImage);
		glBindTexture(GL_TEXTURE_2D, txtbuf[1]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, txtbuf[0]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glog("Image width: " << nWidth << "    Image height: " << nHeight);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(nWidth), GLsizei(nHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)orig);
		const int origSize = nWidth * nHeight * 4;
		for (int i = 0; i < origSize; ++i) pixels[i] = GLfloat(orig[i]);
		GLint invocMax;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &invocMax);
		std::stringstream shader;
		shader
			<< "#version 450\n"
			<< "layout(local_size_x = " << invocMax << ", local_size_y = 1, local_size_z = 1) in;\n"
			<< "layout(binding = 0) buffer InOut0SSBO\n"
			<< "{\n"
			<< "    vec4 data[];\n"
			<< "} pixels;\n"
			<< "layout(binding = 1) buffer InOut1SSBO\n"
			<< "{\n"
			<< "    vec4 data[];\n"
			<< "} map;\n"
			<< "layout(binding = 2) buffer InOut2SSBO\n"
			<< "{\n"
			<< "    uvec2 data[];\n"
			<< "} epochWinners;\n"
			<< "layout(binding = 3) buffer InOut3SSBO\n"
			<< "{\n"
			<< "    double data[];\n"
			<< "} totalDis;\n"
			<< "#define mapAspect " << mapAspect << "\n"
			<< "#define mapSize   " << mapAspect * mapAspect << "\n"
			<< "#define numPixels " << nWidth * nHeight << "\n"
			<< "#define numIters  " << numIter << "\n"
			<< "void main()\n"
			<< "{\n"
			<< "    const uint asgm   = uint(ceil(float(mapSize) / float(gl_WorkGroupSize.x)));\n"
			<< "    const uint asgmlb = asgm * gl_LocalInvocationIndex;\n"
			<< "    const uint asgmub = (asgmlb + asgm < mapSize ? asgmlb + asgm : mapSize);\n"
			<< "    const uint asgp   = uint(ceil(float(numPixels) / float(gl_WorkGroupSize.x)));\n"
			<< "    const uint asgplb = asgp * gl_LocalInvocationIndex;\n"
			<< "    const uint asgpub = (asgplb + asgp < numPixels ? asgplb + asgp : numPixels);\n"
			/////// Normalizing pixels -------------------------------------------------------------
			<< "    for(uint i = asgplb; i < asgpub; ++i)\n"
			<< "    {\n"
			<< "        pixels.data[i] /= 256.0;\n"
			<< "    }\n"
			<< "    barrier();\n" /// \todo can be imporoved
			/////// Algorithm iteration loop begin -------------------------------------------------
			<< "    for(int iter = 0; iter < numIters; ++iter)\n"
			<< "    {\n"
			/////////// Finding winner -------------------------------------------------------------
			<< "        for(uint i = asgplb; i < asgpub; ++i)\n"
			<< "        {\n"
			<< "            float minDis = 999.0;\n"
			<< "            for(uint mi = 0; mi < mapSize; ++mi)\n"
			<< "            {\n"
			<< "                vec4 d = pixels.data[i] - map.data[mi];\n"
			<< "                float dis = dot(d, d);\n"
			<< "                if(dis < minDis)\n"
			<< "                {\n"
			<< "                    minDis = dis;\n"
			<< "                    epochWinners.data[i] = uvec2(mi % mapAspect, mi / mapAspect);\n"
			<< "                }\n"
			<< "            }\n"
			<< "        }\n"
			/////////// Learning Rate Coefficent ---------------------------------------------------
			<< "        float lrc = float(iter * iter) + 1.0f;\n" /// \todo find better function 
			<< "        barrier();\n" /// \todo can be imporoved
			/////////// Updating weights of map nodes ----------------------------------------------
			<< "        for(uint mi = asgmlb; mi < asgmub; ++mi)\n"
			<< "        {\n"
			<< "            uvec2 rk = uvec2(mi % mapAspect, mi / mapAspect);\n"
			<< "            dvec4 numerator = dvec4(0.0);\n"
			<< "            double denominator = 0.0;\n"
			<< "            for(uint pi = 0; pi < numPixels; ++pi)\n"
			<< "            {\n"
			<< "                uvec2 rk_rc = rk - epochWinners.data[pi];\n"
			<< "                float rk_rc2 = -float(dot(rk_rc, rk_rc));\n"
			<< "                float hck = exp(lrc * rk_rc2);\n"
			<< "                numerator += dvec4(pixels.data[pi] * hck);\n"
			<< "                denominator += hck;\n"
			<< "            }\n"
			<< "            map.data[mi] = vec4(numerator / denominator);\n"
			<< "        }\n"
			<< "        barrier();\n" /// \todo can be imporoved
			<< "    }\n"
			/////// Algorithm iteration loop end ---------------------------------------------------
			<< "    for(uint pi = asgplb; pi < asgpub; ++pi)\n"
			<< "    {\n"
			<< "        float minDis = 9999.9;\n"
			<< "        uint wini;\n"
			<< "        for(uint mi = 0; mi < mapSize; ++mi)\n"
			<< "        {\n"
			<< "            float dis = distance(map.data[mi], pixels.data[pi]);\n"
			<< "            if(dis < minDis)\n"
			<< "            {\n"
			<< "                wini = mi;\n"
			<< "                minDis = dis;\n"
			<< "            }\n"
			<< "        }\n"
			<< "        pixels.data[pi] = map.data[wini] * 256.0;\n"
			<< "        totalDis.data[pi] = minDis;\n"
			<< "    }\n"
			<< "    barrier();\n"
			<< "    for(uint mi = asgmlb; mi < asgmub; ++mi)\n"
			<< "    {\n"
			<< "        map.data[mi] *= 256.0;\n"
			<< "    }\n"
			<< "}";
		glog(shader.str());
		GLuint prg = shader::Engine::createProgram();
		GLuint shd = shader::Engine::addShaderToProgram(shader.str(), GL_COMPUTE_SHADER, prg);
		shader::Engine::run(prg);
		GLuint pixelBuf;
		glGenBuffers(1, &pixelBuf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pixelBuf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, origSize * sizeof(GLfloat), pixels, GL_DYNAMIC_READ);
		delete[] pixels;
		GLuint mapBuf;
		glGenBuffers(1, &mapBuf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mapBuf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mapSize * sizeof(GLfloat), map, GL_DYNAMIC_READ);
		delete[] map;
		GLuint epochWinnersBuf;
		glGenBuffers(1, &epochWinnersBuf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, epochWinnersBuf);
		GLuint *epochWinners = new GLuint[nWidth * nHeight * 2];
		glBufferData(GL_SHADER_STORAGE_BUFFER, nWidth * nHeight * sizeof(GLuint) * 2, epochWinners, GL_STATIC_DRAW);
		delete[] epochWinners;
		GLuint algoOutBuf;
		glGenBuffers(1, &algoOutBuf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, algoOutBuf);
		GLdouble *totalDis = new GLdouble[nWidth * nHeight];
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLdouble) * nWidth * nHeight, totalDis, GL_DYNAMIC_READ);
		delete [] totalDis;
		glDispatchCompute(1, 1, 1);
		glFinish();
		glDeleteBuffers(1, &epochWinnersBuf);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixelBuf);
		GLfloat *result = (GLfloat *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		unsigned char *compix = new unsigned char[origSize];
		for (int i = 0; i < origSize; ++i) compix[i] = (unsigned char)(result[i]);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glDeleteBuffers(1, &pixelBuf);
		glBindTexture(GL_TEXTURE_2D, txtbuf[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(nWidth), GLsizei(nHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)compix);
		delete[] compix;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapBuf);
		result = (GLfloat *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		unsigned char *mappix = new unsigned char[mapSize];
		for (int i = 0; i < mapSize; ++i) mappix[i] = (unsigned char)(result[i]);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glDeleteBuffers(1, &mapBuf);
		glGenTextures(1, &txtmapbuf);
		glBindTexture(GL_TEXTURE_2D, txtmapbuf);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(mapAspect), GLsizei(mapAspect), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mappix);
		delete[] mappix;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, algoOutBuf);
		totalDis = (GLdouble *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		long double td = 0.0;
		for (int i = 0; i < nWidth * nHeight; ++i) td += totalDis[i];
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glDeleteBuffers(1, &algoOutBuf);
		FreeImage_Unload(bitmap);
		FreeImage_Unload(pImage);
		glog("Total Distane is: " << td);
	}
	{
		const GLfloat spacing = 0.1f;
		const GLfloat m1[] =
		{
			spacing        , 1.0f, 0.0f,         1.0f, 1.0f,
			spacing        , 0.0f, 0.0f,         1.0f, 0.0f,
			meshW + spacing, 1.0f, 0.0f,         0.0f, 1.0f,
			meshW + spacing, 0.0f, 0.0f,         0.0f, 0.0f,
		};
		const GLfloat m2[] =
		{
			-spacing          , 1.0f, 0.0f,         0.0f, 1.0f,
			-(meshW + spacing), 1.0f, 0.0f,         1.0f, 1.0f,
			-spacing          , 0.0f, 0.0f,         0.0f, 0.0f,
			-(meshW + spacing), 0.0f, 0.0f,         1.0f, 0.0f,
		};
		const GLfloat m3[] =
		{
			-0.5f, -0.1f, 0.0f,         0.0f, 1.0f,
			-0.5f, -0.6f, 0.0f,         1.0f, 1.0f,
			 0.5f, -0.1f, 0.0f,         0.0f, 0.0f,
			 0.5f, -0.6f, 0.0f,         1.0f, 0.0f,
		};
		const GLuint ind[]
		{
			0, 1, 2,
			1, 3, 2
		};
		glGenBuffers(4, bo);
		glBindBuffer(GL_ARRAY_BUFFER, bo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m1), m1, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, bo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m2), m2, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, bo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m3), m3, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);
	}
	{
		shRenPrg = shader::Engine::createProgram();
		std::stringstream shaderSrc;
		shaderSrc
			<< "#version 330\n"
			<< "in vec3 pos;\n"
			<< "in vec2 uv;\n"
			<< "out vec2 txtCoord;\n"
			<< "uniform mat4 mvp;\n"
			<< "void main() {\n"
			<< "    gl_Position = mvp * vec4(pos, 1.0);\n"
			<< "    txtCoord = uv;\n"
			<< "}\n";
		glog(shaderSrc.str());
		GLuint vtxSh = shader::Engine::addShaderToProgram(shaderSrc.str(), GL_VERTEX_SHADER, shRenPrg);
		shaderSrc = std::stringstream();
		shaderSrc
			<< "#version 330\n"
			<< "in vec2 txtCoord;\n"
			<< "out vec4 fragColor;\n"
			<< "uniform sampler2D txtSmp;\n"
			<< "void main() {\n"
			<< "    fragColor = texture2D(txtSmp, txtCoord);\n"
			<< "}\n";
		glog(shaderSrc.str());
		GLuint frgSh = shader::Engine::addShaderToProgram(shaderSrc.str(), GL_FRAGMENT_SHADER, shRenPrg);
		shader::Engine::run(shRenPrg);
		vtxAttInd = glGetAttribLocation(shRenPrg, "pos");
		uvAttInd  = glGetAttribLocation(shRenPrg, "uv");
		mvpLoc = shader::Engine::getUniformLocation("mvp", shRenPrg);
		txtLoc = shader::Engine::getUniformLocation("txtSmp", shRenPrg);
		glEnableVertexAttribArray(vtxAttInd);
		glEnableVertexAttribArray(uvAttInd);
		glVertexAttribPointer(vtxAttInd, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) 0);
		glVertexAttribPointer(uvAttInd,  2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &(camera->getVPM().m[0][0]));
		glUniform1i(txtLoc, 0);
	}
}

void gearoenix::render::Scene::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, bo[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo[3]);
	glUseProgram(shRenPrg);
	glEnableVertexAttribArray(vtxAttInd);
	glEnableVertexAttribArray(uvAttInd);
	glVertexAttribPointer(vtxAttInd, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(uvAttInd, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &(camera->getVPM().m[0][0]));
	glUniform1i(txtLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, txtbuf[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, bo[0]);
	glVertexAttribPointer(vtxAttInd, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(uvAttInd, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindTexture(GL_TEXTURE_2D, txtbuf[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, bo[2]);
	glVertexAttribPointer(vtxAttInd, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(uvAttInd, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindTexture(GL_TEXTURE_2D, txtmapbuf);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gearoenix::render::Scene::moveCamera(const float &dx, const float &dy)
{
	camera->move(math::Vector3Df(-dx, dy, 0.0f));
}

void gearoenix::render::Scene::moveForward(const float &dis)
{
	((math::OrthoCamera *)(camera))->scaleOrthoSize(1.0f - (dis * 0.05f));
}

void gearoenix::render::Scene::setViewport(const float &w, const float &h)
{
	camera->setViewport(w, h);
}