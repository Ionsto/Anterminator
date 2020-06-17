#pragma once
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "RenderEngine.h"
#include "World.h"
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include "KeyManager.h"
#include <glm/glm.hpp>
class GameManager
{
private:
	int FrameCount = 0;
	float FPSAcc = 0;
	std::chrono::time_point<std::chrono::steady_clock> StartTime;
	std::chrono::time_point<std::chrono::steady_clock> DtCounter;
	float DeltaTime = 0;
	bool KeyCapture = true;
	glm::ivec2 ScreenSize;
	glm::vec2 MouseScreenPos;
	glm::vec2 MouseScreenPosOld;
	bool MouseDown = false;
public:
	KeyManager KeyInput;
	float clickticker = 0;
	int clicktickerwater = 0;
	bool Running = true;
	std::unique_ptr<World> world;
	std::unique_ptr<RenderEngine> renderengine;
	GLFWwindow* Window_Handle;
	GameManager();
	~GameManager();
	void Run();
	void Update();
	void PollInput();
	void Render();
};

