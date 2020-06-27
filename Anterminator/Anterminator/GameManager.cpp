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
	Window_Handle = glfwCreateWindow(800, 800, "Anterminator",NULL, NULL);
	ScreenSize = glm::vec2(800, 800);
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
	//renderengine->cam.Position.x = 16 / 2;
	//renderengine->cam.Position.y = 16 / 2;
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
		FPSAcc += DeltaTime;
		FrameCount++;
		if (FPSAcc > 1)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto dt = now - StartTime;
			StartTime = now;
//			std::cout << "Frametime:" << (dt.count() / (FrameCount * 1e9)) << "\n fps:" << ((FrameCount * 1e9) / dt.count()) << "\n";
			std::cout << "Frametime:" << FPSAcc / (float)FrameCount<<"\n";
			std::cout << "FPS:" << FrameCount / FPSAcc <<"\n";
			std::cout << "Total Entity Count:" << world->EntityList.ElementCount << "\n";
			std::cout << "Total Render Count:" << dynamic_cast<RenderEngineGPU*>(renderengine.get())->EntityCounter << "\n";
			std::cout << "Render view size:" << renderengine->cam.CameraXSize<<"\n";
			std::cout << "Render view size:" << static_cast<int>(renderengine->cam.CameraXSize/Chunk::Size)<<" chunks\n";
			//std::cout << "Particle count:" << world->waterengine.ParticleCount << "\n";
			FrameCount = 0;
			FPSAcc = 0;
		}
		auto end = std::chrono::high_resolution_clock::now();
		DeltaTime = (end - DtCounter).count()/1e9;
		DtCounter = end; DtCounter = end;
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
	double xpos, ypos;
	glfwGetCursorPos(Window_Handle, &xpos, &ypos);
	glfwGetWindowSize(Window_Handle, &ScreenSize.x, &ScreenSize.y);
	glm::vec2 mpos(xpos,ScreenSize.y - ypos);
	mpos.x /= ScreenSize.x;
	mpos.y /= ScreenSize.y;
	mpos.x -= 0.5;
	mpos.y -= 0.5;
	//mpos *= 8.0f;
	MouseScreenPosOld = MouseScreenPos;
	MouseScreenPos = renderengine->cam.Position;
	MouseScreenPos += glm::vec2(mpos.x, mpos.y) * renderengine->cam.CameraXSize * 2.0f;
	//MouseScreenPos = glm::clamp(MouseScreenPos,glm::vec2(0, 0), glm::vec2(world->WorldSize, world->WorldSize));
	if (glfwGetMouseButton(Window_Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		int Aff = renderengine->RenderConfig.RenderAffiliation;
		int size = 5;
		glm::vec2 vel = glm::vec2(world->WrapDistance(MouseScreenPos.x, MouseScreenPosOld.x), world->WrapDistance(MouseScreenPos.y , MouseScreenPosOld.y));
		float distance = glm::length(vel);
		int maxi = ceil(distance / 1);
		int lasti = maxi - 1;
		for (int i = 0; i < maxi; ++i)
		{
			for (int x = -size; x <= size; ++x)
			{
				for (int y = -size; y <= size; ++y)
				{
					auto pos = MouseScreenPos - (vel * (float(i) / float(maxi)));
					world->FactionArray[Aff].PheremoneAttack.GetChunk(pos.x + x, pos.y + y).Active = true;
					world->FactionArray[Aff].PheremoneAttack.GetChunk(pos.x + x, pos.y + y).ZeroValue = false;
					world->FactionArray[Aff].PheremoneAttack.GetPheremone(pos.x + x, pos.y + y).Strength = 100;
					//vel = glm::normalize(vel) * 10.0f;
					world->FactionArray[Aff].PheremoneAttack.GetPheremone(pos.x + x, pos.y + y).Direction = vel/DeltaTime;
					//world->ColonyArray[0].Pheremone_Food.GetPheremone(WorldMousePos.x + x, WorldMousePos.y + y).Strength = 100;
					//world->ColonyArray[0].Pheremone_Food.GetPheremone(WorldMousePos.x + x, WorldMousePos.y + y).Direction = WorldMousePos - WorldMousePosOld;
				}
			}
		}
	}
	if (glfwGetMouseButton(Window_Handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (!MouseDown)
		{
			MouseDown = true;
			auto e = world->PrintEntityInfo(MouseScreenPos.x,MouseScreenPos.y);
			if (e.Affiliation >= 0 && e.Affiliation < world->AntFactionCount)
			{
				renderengine->RenderConfig.RenderAffiliation = e.Affiliation;
			}
		}
	}
	else {
		MouseDown = false;
	}
	float speed = 30 * DeltaTime;
	/*if (player != nullptr)
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
	*/
	if (false) 
	{

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
		renderengine->cam.tilt = std::clamp(renderengine->cam.tilt, -70.0f, 70.0f);
		float MoveSpeed = 20;
		/*if (KeyInput.GetState(GLFW_KEY_W).first)
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
		*/
		MoveSpeed *= renderengine->cam.CameraXSize / Chunk::Size;
		if (KeyInput.GetState(GLFW_KEY_W).first)
		{
			renderengine->cam.Position += DeltaTime * MoveSpeed * glm::fvec3(0, 1, 0);
		}
		if (KeyInput.GetState(GLFW_KEY_S).first)
		{
			renderengine->cam.Position -= DeltaTime * MoveSpeed * glm::fvec3(0, 1, 0);
		}
		if (KeyInput.GetState(GLFW_KEY_A).first)
		{
			renderengine->cam.Position -= DeltaTime * MoveSpeed * glm::fvec3(1, 0, 0);
		}
		if (KeyInput.GetState(GLFW_KEY_D).first)
		{
			renderengine->cam.Position += DeltaTime * MoveSpeed * glm::fvec3(1, 0, 0);
		}
		if (KeyInput.GetState(GLFW_KEY_M) == std::pair{ true,true })
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
		if (KeyInput.GetState(GLFW_KEY_0) == std::pair{ true,true })
		{
			renderengine->cam.CameraXSize *= 0.5;
			renderengine->cam.CameraYSize *= 0.5;
		}
		if (KeyInput.GetState(GLFW_KEY_9) == std::pair{ true,true })
		{
			renderengine->cam.CameraXSize /= 0.5;
			renderengine->cam.CameraYSize /= 0.5;
		}
		if (KeyInput.GetState(GLFW_KEY_P) == std::pair{ true,true })
		{
			world->TimeScalingFactor *= 0.5;
			world->DtAccumulator = 0;
			std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
		}
		if (KeyInput.GetState(GLFW_KEY_O) == std::pair{ true,true })
		{
			world->TimeScalingFactor /= 0.5;
			world->DtAccumulator = 0;
			std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
		}
		//renderengine->cam.Position.x = std::clamp(renderengine->cam.Position.x, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
		//renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
		while (renderengine->cam.Position.x < 0)
		{
			renderengine->cam.Position.x += world->WorldSize;
		}
		while (renderengine->cam.Position.x > world->WorldSize)
		{
			renderengine->cam.Position.x -= world->WorldSize;
		}
		while (renderengine->cam.Position.y < 0)
		{
			renderengine->cam.Position.y += world->WorldSize;
		}
		while (renderengine->cam.Position.y > world->WorldSize)
		{
			renderengine->cam.Position.y -= world->WorldSize;
		}
		//renderengine->cam.Position.x = std::clamp(renderengine->cam.Position.x, (float)Chunk::Size,-0.1f + ((float)world->Chunks.WorldSize-1) * Chunk::Size);
		//renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, (float)Chunk::Size,-0.1f +  ((float)world->Chunks.WorldSize-1) * Chunk::Size);
		//renderengine->cam.Position.z = std::clamp(renderengine->cam.Position.z, 0.0f,-0.1f +  (float)Chunk::Size);
	}
	//if (auto[s,t] = KeyInput.GetState(GLFW_KEY_1); s && t) {
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_1)) {
		renderengine->RenderConfig.RenderPheremone ^= true;
		std::cout << "Pheremone home: " << renderengine->RenderConfig.RenderPheremone << std::endl;
	}
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_2)) {
		renderengine->RenderConfig.Refraction ^= true;
		std::cout << "Refraction: " << renderengine->RenderConfig.Refraction << std::endl;
	}
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_R)) {
		world = std::make_unique<World>();
	}
	if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_E)) {
		//int id = world->AddEntity(std::make_unique<Entity>(*world));
		//if (id != -1)
		//{
		//	auto& entity = world->EntityList.GetParticle(id);
		//	entity.Position = glm::vec3(8, 8, 12);
		//	entity.PositionOld = glm::vec3(8, 8, 12);
		//}
	}
	if (KeyInput.GetState(GLFW_KEY_ESCAPE).first)
	{
		Running = false;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_2)){
		renderengine->RenderConfig.Pheremone = 0;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_3)){
		renderengine->RenderConfig.Pheremone = 1;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_4)){
		renderengine->RenderConfig.Pheremone = 2;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_5)){
		renderengine->RenderConfig.Pheremone = 3;
	}
	if (glfwGetKey(Window_Handle, GLFW_KEY_6)){
		renderengine->RenderConfig.Pheremone = 4;
	}

}
void GameManager::Update()
{
	if (DeltaTime > 1/100.0) 
	{
//		world->DtAccumulator = 0;
		world->TimeScalingFactor /= 2;
		//std::cout << "Auto slow down\n";
		//std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
	}
	if (DeltaTime < 1/1000.0) 
	{
		//world->DtAccumulator = 0;
		world->TimeScalingFactor *= 2;
		//std::cout << "Auto speed up\n";
		//std::cout << "Time scale:" << world->TimeScalingFactor << "\n";
	}
	PollInput();
//	renderengine->cam.yaw += DeltaTime * 30;
	//this->waterengine->Update(DeltaTime);
	world->Update(DeltaTime);
	if (glfwWindowShouldClose(Window_Handle)) {
		Running = false;
	}
}