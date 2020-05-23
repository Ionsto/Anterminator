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
#include "EntityPlayer.h"
#include "KeyManager.h"
class GameManager
{
private:
	int FrameCount = 0;
	std::chrono::time_point<std::chrono::steady_clock> StartTime;
	std::chrono::time_point<std::chrono::steady_clock> DtCounter;
	float DeltaTime = 0;
	bool KeyCapture = true;
public:
	KeyManager KeyInput;
	EntityPlayer* player = nullptr;
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

