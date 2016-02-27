
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

static GLuint bo[3];
static GLuint shRenPrg;
static GLuint vtxAttInd;
static GLuint uvAttInd;
static GLuint mvpLoc;
static GLuint txtLoc;
static GLuint txtbuf[2];

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
		float *data = new float[nWidth * nHeight * 4];
		const int mapSize = mapAspect * mapAspect * 4;
		float *map = new float[mapSize];
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);
		for (int i = 0; i < mapSize; ++i)
			map[i] = uniform_dist(e1);
		unsigned char *orig = (unsigned char *)FreeImage_GetBits(pImage);
		glBindTexture(GL_TEXTURE_2D, txtbuf[0]); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glog("Image width: " << nWidth << "    Image height: " << nHeight);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(nWidth), GLsizei(nHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)orig);
		const int origSize = nWidth * nHeight * 4;
		for (int i = 0; i < origSize; ++i)
			data[i] = float(orig[i]);
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
			<< "    vec3 data[];\n"
			<< "} map;\n"
			<< "#define mapAspect " << mapAspect << "\n"
			<< "#define numPixels " << origSize << "\n"
			<< "#define numIters  " << numIter << "\n"
			<< "void main()\n"
			<< "{\n"
			<< "    const uint asgm   = uint(ceil(float(mapAspect) / float(gl_WorkGroupSize.x)));\n"
			<< "    const uint asgmlb = asgm * gl_LocalInvocationIndex;\n"
			<< "    const uint asgmub = (asgmlb + asgm < mapAspect ? asgmlb + asgm : mapAspect);\n"
			<< "    const uint asgp   = uint(ceil(float(numPixels) / float(gl_WorkGroupSize.x)));\n"
			<< "    const uint asgplb = asgp * gl_LocalInvocationIndex;\n"
			<< "    const uint asgpub = (asgplb + asgp < numPixels ? asgplb + asgp : numPixels);\n"
			 //Normalizing pixels ------------------------------------------------------------------
			<< "    for(uint i = asgplb; i < asgpub; ++i)\n"
			<< "    {\n"
			<< "        pixels.data[i] /= 256.0;\n"
			<< "    }\n"
			<< "}";
		GLuint prg = shader::Engine::createProgram();
		GLuint shd = shader::Engine::addShaderToProgram(shader.str(), GL_COMPUTE_SHADER, prg);
		shader::Engine::run(prg);
		GLuint pixelBuf;
		glGenBuffers(1, &pixelBuf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pixelBuf);
		glBufferData(GL_SHADER_STORAGE_BUFFER, origSize * sizeof(GLfloat), data, GL_DYNAMIC_READ);
		glDispatchCompute(1, 1, 1);
		glFinish();
		float *result = (float *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		{ // debuging
			for (int i = 0; i < origSize; i++)
			{
				if (result[i] != data[i] / 256.0f)
					core::Logger::err << i << "- " << result[i] << " <- " << data[i] << "\n";
			}
			core::Logger::err.flush();
		}
		glDeleteBuffers(1, &pixelBuf);
		delete[] map;
		delete[] data;
		FreeImage_Unload(bitmap);
		FreeImage_Unload(pImage);
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
		const GLuint ind[]
		{
			0, 1, 2,
			1, 3, 2
		};
		glGenBuffers(3, bo);
		glBindBuffer(GL_ARRAY_BUFFER, bo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m1), m1, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, bo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m2), m2, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo[2]);
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo[2]);
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