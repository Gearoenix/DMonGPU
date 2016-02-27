#ifndef GEAROENIX_SHADER_ENGINE
#define GEAROENIX_SHADER_ENGINE
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
namespace gearoenix
{
	namespace shader
	{
		class Engine
		{
		private:
		public:
			static GLuint createProgram();
			static void run(const GLuint &shaderProgram);
			static void link(const GLuint &shaderProgram);
			static void validate(const GLuint &shaderProgram);
			static GLuint addShaderToProgram(const std::string &shd, const GLenum &shaderType, const GLuint &shaderProgram);
			static GLuint getUniformLocation(const std::string &uname, const GLuint &shaderProgram);
			static void endProgram(const GLuint &shaderProgram);
			static void endObject(const GLuint &shaderObject);
			static GLint getAttributeLocation(const GLuint &shdProg, const char *const &name);
		};
	}
}
#endif