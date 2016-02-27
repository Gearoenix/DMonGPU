#include "gearoenix-shader-engine.hpp"
#include "gearoenix-log.hpp"
#include "gearoenix-shortcuts.hpp"
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <vector>

GLuint gearoenix::shader::Engine::createProgram()
{
	GLuint shaderProgram = glCreateProgram();
	if (shaderProgram == 0)
	{
		gce("Error creating shader program.");
	}
	return shaderProgram;
}

GLuint gearoenix::shader::Engine::addShaderToProgram(const std::string &shd, const GLenum &shaderType, const GLuint &shaderProgram)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0)
	{
		gce("Error creating shader type.");
	}
	const char *chtemp = &(shd[0]);
	const GLint uintemp = (GLint)shd.length();
	glShaderSource(shaderObj, 1, &(chtemp), &(uintemp));
	glCompileShader(shaderObj);
	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &maxLength);
		std::string infoLog;
		infoLog.resize(maxLength);
		glGetShaderInfoLog(shaderObj, 10000, NULL, &infoLog[0]);
		gce("Error compiling shader type. Info: " << infoLog);
	}
	glAttachShader(shaderProgram, shaderObj);
	return shaderObj;
}

void gearoenix::shader::Engine::link(const GLuint &shaderProgram)
{
	GLint isSuccess = (GLint)0;
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isSuccess);
	if (isSuccess == 0) {
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		std::string infoLog;
		infoLog.resize(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
		gce("Error linking shader program: " << infoLog);
	}
}

void gearoenix::shader::Engine::validate(const GLuint &shaderProgram)
{
	GLint isSuccess = (GLint)0;
	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &isSuccess);
	if (!isSuccess) {
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		std::string infoLog;
		infoLog.resize(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
		glog("Invalid shader program: " << infoLog);
	}
	glUseProgram(shaderProgram);
}

void gearoenix::shader::Engine::run(const GLuint &shaderProgram)
{
	link(shaderProgram);
	validate(shaderProgram);
}

GLuint gearoenix::shader::Engine::getUniformLocation(const std::string &uname, const GLuint &shaderProgram)
{
	GLint result = glGetUniformLocation(shaderProgram, &(uname[0]));
	if (result == -1)
	{
		gce("Error in finding uniform variable: " << uname);
	}
	return result;
}

void gearoenix::shader::Engine::endProgram(const GLuint &shaderProgram)
{
	glDeleteProgram(shaderProgram);
}

void gearoenix::shader::Engine::endObject(const GLuint &shaderObject)
{
	glDeleteShader(shaderObject);
}

GLint gearoenix::shader::Engine::getAttributeLocation(const GLuint &shdProg, const char *const &name)
{
	GLint result = glGetAttribLocation(shdProg, name);
	if (result == -1)
	{
		gce("Error in finding attribute: " << name);
	}
	return result;
}
