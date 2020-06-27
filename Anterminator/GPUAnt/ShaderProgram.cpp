#include "ShaderProgram.h"
#include <vector>
#include <iostream>

ShaderProgram::ShaderProgram()
{
}

void ShaderProgram::CreateProgram()
{
	ProgramID = glCreateProgram();
}


ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(ProgramID);
}
void ShaderProgram::AddShader(Shader shader)
{
	glAttachShader(ProgramID, shader.ShaderID);
}
void ShaderProgram::LinkProgram()
{
	glLinkProgram(ProgramID);
	GLint isLinked = 0;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(ProgramID, maxLength, &maxLength, &infoLog[0]);
		std::cout << &infoLog[0] << std::endl;
	}
	else
	{
		std::cout << "Shader linked properly" << std::endl;
	}
}
void ShaderProgram::UseProgram()
{
	glUseProgram(ProgramID);
}
