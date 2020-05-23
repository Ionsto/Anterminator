#pragma once
#include <string>
#include <GL/glew.h>
class Shader
{
public:
	GLuint ShaderID;
	bool Init(std::string location, GLenum shaderType);
	Shader();
	~Shader();
};

