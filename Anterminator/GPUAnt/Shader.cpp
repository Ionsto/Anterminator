#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Shader::Shader()
{
}


Shader::~Shader()
{
	glDeleteShader(ShaderID);
}
bool Shader::Init(std::string location, GLenum shaderType)
{
	std::cout << "Loading shader:" << location << std::endl;
	std::ifstream input(location);
	std::stringstream buffer;
	if (!input.is_open())
	{
		std::cout << "Could not open shader at:" << location << std::endl;
		return false;
	}
	buffer << input.rdbuf();
	std::string shadersource = buffer.str();
	ShaderID = glCreateShader(shaderType);
	const char * src = shadersource.c_str();
	glShaderSource(ShaderID, 1, &src, NULL);
	glCompileShader(ShaderID);
	// Check the result of the compilation
	GLint test;
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &test);
	if (test == GL_FALSE) {
		std::cout << "Shader compilation failed with this message:" << std::endl;
		std::vector<char> compilation_log(512);
		glGetShaderInfoLog(ShaderID, compilation_log.size(), NULL, &compilation_log[0]);
		std::cout << &compilation_log[0] << std::endl;
		return false;
	}
	std::cout << "Shader compiled correctly" << std::endl;
	return true;
}