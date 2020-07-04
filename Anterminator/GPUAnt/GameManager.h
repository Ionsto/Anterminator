#pragma once

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "WaterEngineGPU.h"
#include "RenderEngine.h"

#include <memory>
#include <chrono>
#include "KeyManager.h"
#include <glm/glm.hpp>
#include "GUIManager.h"
#include "AudioEngine.h"
class GameManager
{
private:
	int FrameCount = 0;
	float FPSAcc = 0;
	std::chrono::time_point<std::chrono::steady_clock> StartTime;
	std::chrono::time_point<std::chrono::steady_clock> DtCounter;
	float DeltaTime = 0;
	bool KeyCapture = false;
	glm::ivec2 ScreenSize;
	glm::vec2 MouseScreenPos;
	glm::vec2 MouseScreenPosOld;
	bool MouseDown = false;
public:
	KeyManager KeyInput;
	float clickticker = 0;
	int clicktickerwater = 0;
	bool Running = true;
	std::unique_ptr<RenderEngine> renderengine;
	std::unique_ptr<WaterEngineGPU> entityengine;
	std::unique_ptr<GUIManager> guimanager;
	std::unique_ptr<AudioEngine> audioengine;
	GLFWwindow* Window_Handle;
	std::array<float, 10> DTstack = {0};
	int DTStackpointer = 0;
	float MeanDT = 0;
	GameManager();
	~GameManager();
	void Run();
	void Update();
	void PollInput();
	void Render();
};

