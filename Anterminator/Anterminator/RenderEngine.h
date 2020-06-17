#pragma once
#include "World.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
struct RenderConfig {
	bool Reflections = false;
	bool Shadows = true;
	bool Refraction = true;
	bool RenderPheremone = false;
	int Pheremone = 0;
	int RenderAffiliation = 0;
};

class RenderEngine
{
protected:
	GLFWwindow* Window;
public:
	RenderConfig RenderConfig;
	Camera cam;
	RenderEngine(GLFWwindow* Window);
	virtual void RenderWorld(World& world);
};

