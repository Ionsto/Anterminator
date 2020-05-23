#pragma once
#include "World.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
struct RenderConfig {
	bool Reflections = false;
	bool Shadows = true;
	bool Refraction = true;
};

class RenderEngine
{
private:
	GLFWwindow* Window;
public:
	RenderConfig RenderConfig;
	Camera cam;
	RenderEngine(GLFWwindow* Window);
	virtual void RenderWorld(World& world);
};

