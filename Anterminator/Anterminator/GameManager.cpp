#include "GameManager.h"
#include "RenderEngineGPU.h"
#include <iostream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
GameManager::GameManager()
{
	std::cout << "Attempting to init glfw" << std::endl;
	if (!glfwInit())
	{
		std::cout << "Couldn't init glfw" << std::endl;
		// Initialization failed
		throw;
	}
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_VERSION_MINOR, 2);;
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	std::cout << "Loading window" << std::endl;
	//Window_Handle = glfwCreateWindow(1920, 1080, "Beaver game", glfwGetPrimaryMonitor(), NULL);
	Window_Handle = glfwCreateWindow(800, 800, "Beaver game",NULL, NULL);
	if (!Window_Handle)
	{
		std::cout << "Window load failed" << std::endl;
		throw;
	}
	std::cout << "Setting up opengl" << std::endl;
	glfwSetFramebufferSizeCallback(Window_Handle, framebuffer_size_callback);
	glfwMakeContextCurrent(Window_Handle);
	glfwSwapInterval(0);
	std::cout << "Init glew" << std::endl;
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW! I'm out!" << std::endl;
		throw;
	}
	std::cout << "Init world" << std::endl;
	//	waterengine = std::make_unique<WaterEngine>();
	world = std::make_unique<World>();
	std::cout << "Init GPU sys" << std::endl;
//	world->waterengine.Init(Window_Handle);
	//world->AddWater(glm::dvec2(100,100));
	std::cout << "Init render engine" << std::endl;
	renderengine = std::make_unique<RenderEngineGPU>(Window_Handle);
	renderengine->cam.Position.x = 16 / 2;
	renderengine->cam.Position.y = 16 / 2;
	int width, height;
	glfwGetFramebufferSize(Window_Handle, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetInputMode(Window_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


GameManager::~GameManager()
{
	glfwDestroyWindow(Window_Handle);
	glfwTerminate();
}

void GameManager::Run()
{
	glfwSwapBuffers(Window_Handle);
	std::cout << "Starting game" << std::endl;
	DtCounter = std::chrono::high_resolution_clock::now();
	while (Running)
	{
		Update();
		Render();
		glfwSwapBuffers(Window_Handle);
		glfwPollEvents();
		if (FrameCount++ == 100)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto dt = now - StartTime;
			StartTime = now;
			std::cout << "Frametime:" << (dt.count() / (100.0 * 1000000000)) << "\n fps:" << ((100.0 * 1000000000) / dt.count()) << "\n";
			//std::cout << "Particle count:" << world->waterengine.ParticleCount << "\n";
			FrameCount = 0;
		}
		auto end = std::chrono::high_resolution_clock::now();
		DeltaTime = (end - DtCounter).count()/1e9;
		DtCounter = end;
	}
}

void GameManager::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderengine->RenderWorld(*world);
	/*renderengine->RotateCounter += renderengine->DRotate * DeltaTime;
	if (renderengine->RotateCounter > 360) {
		renderengine->RotateCounter -= 360;
	}*/
	//world->waterengine.Render();
}
void GameManager::PollInput()
{
	KeyInput.UpdateState(Window_Handle);
	if (glfwGetMouseButton(Window_Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(Window_Handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
	}
	else {
	}
	float speed = 30 * DeltaTime;
	if (player != nullptr)
	{
		player->PlayerInput.W = KeyInput.GetState(GLFW_KEY_W).first;
		player->PlayerInput.S = KeyInput.GetState(GLFW_KEY_S).first;
		player->PlayerInput.A = KeyInput.GetState(GLFW_KEY_A).first;
		player->PlayerInput.D = KeyInput.GetState(GLFW_KEY_D).first;
		double xpos, ypos;
		glfwGetCursorPos(Window_Handle, &xpos, &ypos);
		glfwSetCursorPos(Window_Handle, 0, 0);
		player->PlayerInput.dx = xpos;
		player->PlayerInput.dy = xpos;
	}
	else {
		double xpos, ypos;
		if (KeyCapture)
		{
			glfwGetCursorPos(Window_Handle, &xpos, &ypos);
			glfwSetCursorPos(Window_Handle, 0, 0);
			speed = DeltaTime * 50;
			renderengine->cam.tilt += ypos * speed;
			renderengine->cam.yaw += xpos * speed;
		}
		renderengine->cam.tilt = std::clamp(renderengine->cam.tilt,-70.0f,70.0f);
		float MoveSpeed = 5;
		if (KeyInput.GetState(GLFW_KEY_W).first)
		{
			renderengine->cam.Position += DeltaTime * MoveSpeed * glm::fvec3(cos(glm::radians(renderengine->cam.yaw)),sin(glm::radians(renderengine->cam.yaw)),0);
		}
		if (KeyInput.GetState(GLFW_KEY_S).first)
		{
			renderengine->cam.Position -= DeltaTime * MoveSpeed * glm::fvec3(cos(glm::radians(renderengine->cam.yaw)),sin(glm::radians(renderengine->cam.yaw)),0);
		}
		if (KeyInput.GetState(GLFW_KEY_A).first)
		{
			renderengine->cam.Position += DeltaTime * MoveSpeed * glm::fvec3(sin(glm::radians(renderengine->cam.yaw)),-cos(glm::radians(renderengine->cam.yaw)),0);
		}
		if (KeyInput.GetState(GLFW_KEY_D).first)
		{
			renderengine->cam.Position -= DeltaTime * MoveSpeed * glm::fvec3(sin(glm::radians(renderengine->cam.yaw)),-cos(glm::radians(renderengine->cam.yaw)),0);
		}
		if (KeyInput.GetState(GLFW_KEY_M) == std::pair{true,true})
		{
			if (KeyCapture)
			{
				glfwSetInputMode(Window_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(Window_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(Window_Handle, 0, 0);
			}
			KeyCapture = !KeyCapture;
		}
		renderengine->cam.Position.x = std::clamp(renderengine->cam.Position.x, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
		renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
		//renderengine->cam.Position.x = std::clamp(renderengine->cam.Position.x, (float)Chunk::Size,-0.1f + ((float)world->Chunks.WorldSize-1) * Chunk::Size);
		//renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, (float)Chunk::Size,-0.1f +  ((float)world->Chunks.WorldSize-1) * Chunk::Size);
		renderengine->cam.Position.z = std::clamp(renderengine->cam.Position.z, 0.0f,-0.1f +  (float)Chunk::Size);
	}
	//if (auto[s,t] = KeyInput.GetState(GLFW_KEY_1); s && t) {
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_1)) {
		renderengine->RenderConfig.Reflections ^= true;
		std::cout << "Reflections: " << renderengine->RenderConfig.Reflections << std::endl;
	}
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_2)) {
		renderengine->RenderConfig.Refraction ^= true;
		std::cout << "Refraction: " << renderengine->RenderConfig.Refraction << std::endl;
	}
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_E)) {
		int id = world->AddEntity(std::make_unique<Entity>(*world));
		if (id != -1)
		{
			auto& entity = world->EntityList.GetParticle(id);
			entity.Position = glm::vec3(8, 8, 12);
			entity.PositionOld = glm::vec3(8, 8, 12);
		}
	}
	if (KeyInput.GetState(GLFW_KEY_ESCAPE).first)
	{
		Running = false;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_2)){
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_3)){
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_4)){
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_5)){
	}

}
void GameManager::Update()
{
	PollInput();
//	renderengine->cam.yaw += DeltaTime * 30;
	//this->waterengine->Update(DeltaTime);
	world->Update(DeltaTime);
	if (glfwWindowShouldClose(Window_Handle)) {
		Running = false;
	}
}