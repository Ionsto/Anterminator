#pragma once
#include <GL/glew.h>
#include "Shader.h"
class ShaderProgram
{
private:
	GLint ProgramID = -1;
public:
	ShaderProgram();
	~ShaderProgram();
	void CreateProgram();
	void AddShader(Shader shader);
	void LinkProgram();
	void UseProgram();
	GLint GetProgram()
	{
		return ProgramID;
	}
};

