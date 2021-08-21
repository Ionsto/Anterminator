#include "GameManager.h"
#include <iostream>
#include <functional>
#include "GUIGame.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool Scrolled = false;
float ScrollAmount = 0;
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Scrolled = true;
	ScrollAmount += yoffset;
}
/*
void GLAPIENTRY
MessageCallback_GPU(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}*/
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	//if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION || type == GL_DEBUG_TYPE_PERFORMANCE) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
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
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	std::cout << "Loading window" << std::endl;
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	ScreenSize = glm::vec2(1920, 1080);
	ScreenSize = glm::vec2(800, 800);
	Window_Handle = glfwCreateWindow(ScreenSize.x, ScreenSize.y, "Anterminator",NULL, NULL);
	//Window_Handle = glfwCreateWindow(1920, 1080, "Anterminator", glfwGetPrimaryMonitor(), NULL);
//	Window_Handle = glfwCreateWindow(800, 800, "Anterminator",NULL, NULL);
	
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
	glDebugMessageCallback(glDebugOutput, 0);
//	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	std::cout << "Init world" << std::endl;
	//	waterengine = std::make_unique<WaterEngine>();
	std::cout << "Init GPU sys" << std::endl;
//	world->waterengine.Init(Window_Handle);
	//world->AddWater(glm::dvec2(100,100));
	std::cout << "Init render engine" << std::endl;
	renderengine = std::make_unique<RenderEngine>(Window_Handle);
	entityengine = std::make_unique<WaterEngineGPU>();
	entityengine->Init(Window_Handle);
	entityengine->FollowEntity = entityengine->InitWorld();
	renderengine->SetEntityEngine(*entityengine.get());
	renderengine->Init();
	guimanager = std::make_unique<GUIManager>(Window_Handle);
	audioengine = std::make_unique<AudioEngine>();
	//renderengine->cam.Position.x = entityengine->WorldEntityMirror[FollowEntity].PositionX;
	//renderengine->cam.Position.y = entityengine->WorldEntityMirror[FollowEntity].PositionY;
	//FollowEntity = -1;
	//renderengine->cam.Position.x = 16 / 2;
	//renderengine->cam.Position.y = 16 / 2;
	int width, height;
	glfwGetFramebufferSize(Window_Handle, &width, &height);
	glViewport(0, 0, width, height);
	//GLFWscrollfun scroll = std::bind(&GameManager::ScrollCallback, &*this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
	glfwSetScrollCallback(Window_Handle, ScrollCallback);
//	glfwSetInputMode(Window_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		PollInput();
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
			std::cout << "Mean dt:" << MeanDT <<"\n";
			std::cout << "Total Entity Count:" << entityengine->ParticleCount << "\n";
//			std::cout << "Total Render Count:" << dynamic_cast<RenderEngineGPU*>(renderengine.get())->EntityCounter << "\n";
			std::cout << "Render view size:" << renderengine->cam.CameraXSize<<"\n";
//			std::cout << "Render view size:" << static_cast<int>(renderengine->cam.CameraXSize/Chunk::Size)<<" chunks\n";
			//std::cout << "Particle count:" << world->waterengine.ParticleCount << "\n";
			if (entityengine->FollowEntity != -1)
			{
				entityengine->PrintEntityInfoId(entityengine->FollowEntity);
			}
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
	if(guimanager->CurrentGui == GUIManager::GUIStates::GUIGame ||guimanager->CurrentGui == GUIManager::GUIStates::GUIPauseMenu){
		renderengine->EntityCount = entityengine->ParticleCount;
		renderengine->Render();
	}
	guimanager->Render();
}

void GameManager::PollInput()
{
	KeyInput.UpdateState(Window_Handle);
	if (guimanager->CurrentGui == GUIManager::GUIStates::GUIGame) {
		double xpos, ypos;
		glfwGetCursorPos(Window_Handle, &xpos, &ypos);
		glfwGetCursorPos(Window_Handle, &xpos, &ypos);
		glfwGetWindowSize(Window_Handle, &ScreenSize.x, &ScreenSize.y);
		glm::vec2 mpos(xpos, ScreenSize.y - ypos);
		mpos.x /= ScreenSize.x;
		mpos.y /= ScreenSize.y;
		mpos.x -= 0.5;
		mpos.y -= 0.5;
		mpos *= 2.0;
//		mpos *= 1080;
		mpos.x *= 16.0 / 9.0;
		double viewRatio = tan(((float)3.14 / (180.f / renderengine->cam.FOV) / 2.00f));
		mpos *= viewRatio;
		auto loc = (mpos/renderengine->cam.ClipNear) * renderengine->cam.Position.z;
		auto wpos = glm::vec2(loc.x + renderengine->cam.Position.x, loc.y + renderengine->cam.Position.y);
		MouseScreenPosOld = MouseScreenPos;
		MouseScreenPos = wpos;// +glm::vec2(renderengine->cam.Position.x, renderengine->cam.Position.y);
		//std::cout << "pos" << wpos.x <<","<<wpos.y << "\n";
		//MouseScreenPos += glm::vec2(mpos.x * renderengine->cam.CameraXSize * 2.0f, mpos.y * renderengine->cam.CameraYSize * 2.0f);
		//MouseScreenPosOld = MouseScreenPos;
		//MouseScreenPos = renderengine->cam.Position;
		//MouseScreenPos += glm::vec2(mpos.x * renderengine->cam.CameraXSize * 2.0f, mpos.y * renderengine->cam.CameraYSize * 2.0f);
		if (glfwGetMouseButton(Window_Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			int Aff = renderengine->RenderConfig.RenderAffiliation;
			int size = 10 * renderengine->cam.CameraXSize/1920.0;
			glm::vec2 vel = glm::vec2(entityengine->WrapDistance(MouseScreenPos.x, MouseScreenPosOld.x), entityengine->WrapDistance(MouseScreenPos.y, MouseScreenPosOld.y));
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
						int id = entityengine->GetPheremoneIndex(pos.x + x, pos.y + y);
						entityengine->PheremonePainterDirty = true;
						entityengine->PheremonePainter[id] = vel.x / DeltaTime;
						entityengine->PheremonePainter[id + 1] = vel.y / DeltaTime;
						//					world->FactionArray[Aff].PheremoneAttack.GetChunk(pos.x + x, pos.y + y).Active = true;
						//					world->FactionArray[Aff].PheremoneAttack.GetChunk(pos.x + x, pos.y + y).ZeroValue = false;
						//					world->FactionArray[Aff].PheremoneAttack.GetPheremone(pos.x + x, pos.y + y).Strength = 100;
											//vel = glm::normalize(vel) * 10.0f;
						//					world->FactionArray[Aff].PheremoneAttack.GetPheremone(pos.x + x, pos.y + y).Direction = vel/DeltaTime;
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
				auto i = entityengine->PrintEntityInfo(MouseScreenPos.x, MouseScreenPos.y);
				entityengine->FollowEntity = i;
				if (i != -1) {
					GPUEntity e = entityengine->WorldEntityMirror[i];
					if (e.Affiliation >= 0 && e.Affiliation < entityengine->FactionCount)
					{
						renderengine->RenderConfig.RenderAffiliation = e.Affiliation;
					}
				}
			}
		}
		else {
			MouseDown = false;
		}
		float speed = 30 * DeltaTime;
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
			MoveSpeed *= renderengine->cam.CameraXSize / 10;
			MoveSpeed *= MeanDT;
			if (KeyInput.GetState(GLFW_KEY_W).first)
			{
				renderengine->cam.Position += MoveSpeed * glm::fvec3(0, 1, 0);
			}
			if (KeyInput.GetState(GLFW_KEY_S).first)
			{
				renderengine->cam.Position -= MoveSpeed * glm::fvec3(0, 1, 0);
			}
			if (KeyInput.GetState(GLFW_KEY_A).first)
			{
				renderengine->cam.Position -= MoveSpeed * glm::fvec3(1, 0, 0);
			}
			if (KeyInput.GetState(GLFW_KEY_D).first)
			{
				renderengine->cam.Position += MoveSpeed * glm::fvec3(1, 0, 0);
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
			if (Scrolled)
			{
				float mult = ScrollAmount * 0.5;
				renderengine->cam.CameraXSize /= powf(2,mult);
				renderengine->cam.CameraYSize /= powf(2,mult);
				Scrolled = false;
				ScrollAmount = 0;
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
			renderengine->cam.CameraXSize = std::clamp(renderengine->cam.CameraXSize, 1.0f / (1 << 3), entityengine->WorldSize);
			renderengine->cam.CameraYSize = std::clamp(renderengine->cam.CameraYSize, (1.0f / (1 << 3)) * (9.0f / 16.0f), entityengine->WorldSize * (9.0f / 16.0f));
			renderengine->cam.Position.z = (renderengine->cam.CameraXSize / entityengine->WorldSize) * 1000;
			if (KeyInput.GetState(GLFW_KEY_P) == std::pair{ true,true })
			{
				entityengine->TimeScalingFactor *= 0.5;
				entityengine->DtAccumulator = 0;
				std::cout << "Time scale:" << entityengine->TimeScalingFactor << "\n";
			}
			if (KeyInput.GetState(GLFW_KEY_O) == std::pair{ true,true })
			{
				entityengine->TimeScalingFactor /= 0.5;
				entityengine->DtAccumulator = 0;
				std::cout << "Time scale:" << entityengine->TimeScalingFactor << "\n";
			}
			if (KeyInput.GetState(GLFW_KEY_I) == std::pair{ true,true })
			{
				GamePause = !GamePause;
				std::cout << "Game pause state:" << GamePause << "\n";
			}
			//renderengine->cam.Position.x = std::clamp(renderengine->cam.Position.x, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
			//renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, 0.0f,((float)world->Chunks.WorldSize) * Chunk::Size);
			for (int i = 0; i < 2; ++i)
			{
				while (renderengine->cam.Position[i] < -entityengine->WorldSize)
				{
					renderengine->cam.Position[i] += 2 * entityengine->WorldSize;
				}
				while (renderengine->cam.Position[i] >= entityengine->WorldSize)
				{
					renderengine->cam.Position[i] -= 2 * entityengine->WorldSize;
				}
			}
			float height = 0;
			float sampledheight = 0;
			int ixa = floor((entityengine->WrapValue(renderengine->cam.Position.x) + entityengine->WorldSize) / entityengine->WorldTextureResolution);
			int iya = floor((entityengine->WrapValue(renderengine->cam.Position.y) + entityengine->WorldSize) / entityengine->WorldTextureResolution);
			height = std::max(renderengine->cam.Position.z,51.0f);
			int samplesize = 5;
			for (int dy = -samplesize; dy <= samplesize; ++dy)
			{
				for (int dx = -samplesize; dx <= samplesize; ++dx)
				{
					int ix = ixa + dx;
					int iy = iya + dy;
					if (ix < 0) { ix += entityengine->WorldTextureSize; }
					if (iy < 0) { iy += entityengine->WorldTextureSize; }
					if (ix >= entityengine->WorldTextureSize) { ix -= entityengine->WorldTextureSize; }
					if (iy >= entityengine->WorldTextureSize) { iy -= entityengine->WorldTextureSize; }
					int id = ix + (iy * entityengine->WorldTextureSize);
					sampledheight = (entityengine->WorldNoiseMask[id] * 2000) - 1 + 51;
					height =  std::max(height, sampledheight);
				}
			}
			renderengine->cam.Position.z = height;
			//renderengine->cam.Position.y = std::clamp(renderengine->cam.Position.y, (float)Chunk::Size,-0.1f +  ((float)world->Chunks.WorldSize-1) * Chunk::Size);
			//renderengine->cam.Position.z = std::clamp(renderengine->cam.Position.z, 0.0f,-0.1f +  (float)Chunk::Size);
		}
		//if (auto[s,t] = KeyInput.GetState(GLFW_KEY_1); s && t) {
		if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_1)) {
			renderengine->RenderConfig.RenderPheremone ^= true;
			std::cout << "Pheremone home: " << renderengine->RenderConfig.RenderPheremone << std::endl;
		}
		//if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_U)) {
		//	renderengine->RenderConfig.Refraction ^= true;
		//	std::cout << "Refraction: " << renderengine->RenderConfig.Refraction << std::endl;
		//}
		if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_R)) {
			entityengine->FollowEntity = entityengine->ResetWorld();
			static_cast<GUIGame&>(guimanager->GetGUI(GUIManager::GUIStates::GUIGame)).CurrentState = GUIGame::GameState();
			renderengine->cam.Position = glm::vec3(0, 0, 0);
			renderengine->cam.Position.x = entityengine->WorldEntityMirror[entityengine->FollowEntity].PositionX;
			renderengine->cam.Position.y = entityengine->WorldEntityMirror[entityengine->FollowEntity].PositionY;
			//		world = std::make_unique<World>();
		}
		if (std::pair{ true,true } == KeyInput.GetState(GLFW_KEY_U)) {
			renderengine->RenderConfig.DrawMask ^= true;
			std::cout << "Draw mask: " << renderengine->RenderConfig.DrawMask << std::endl;
			//int id = world->AddEntity(std::make_unique<Entity>(*world));
			//if (id != -1)
			//{
			//	auto& entity = world->EntityList.GetParticle(id);
			//	entity.Position = glm::vec3(8, 8, 12);
			//	entity.PositionOld = glm::vec3(8, 8, 12);
			//}
		}
		if (glfwGetKey(Window_Handle, GLFW_KEY_2)) {
			renderengine->RenderConfig.Pheremone = 0;
		}
		if (glfwGetKey(Window_Handle, GLFW_KEY_3)) {
			renderengine->RenderConfig.Pheremone = 1;
		}
		if (glfwGetKey(Window_Handle, GLFW_KEY_4)) {
			renderengine->RenderConfig.Pheremone = 2;
		}
		if (glfwGetKey(Window_Handle, GLFW_KEY_5)) {
			renderengine->RenderConfig.Pheremone = 3;
		}
		if (glfwGetKey(Window_Handle, GLFW_KEY_6)) {
			//		renderengine->RenderConfig.Pheremone = 4;
		}
	}
	if (KeyInput.GetState(GLFW_KEY_ESCAPE).first)
	{
		Running = false;
	}

}
void GameManager::Update()
{
	MeanDT -= DTstack[DTStackpointer];
	DTstack[DTStackpointer] = DeltaTime / DTstack.size();
	MeanDT += DTstack[DTStackpointer];
	DTStackpointer = (DTStackpointer + 1) % DTstack.size();
	if (entityengine->TimeScalingFactor > 1 && MeanDT > 1/30.0 && DTStackpointer == 0) 
	{
		//entityengine->DtAccumulator = 0;
		entityengine->TimeScalingFactor /= 1.2;
		std::cout << "Auto slow down\n";
		std::cout << "Time scale:" << entityengine->TimeScalingFactor << "\n";
	}
/*
	if (entityengine->TimeScalingFactor < 1 &&  MeanDT < 1/100.0 && DTStackpointer == 0)
	{
		//world->DtAccumulator = 0;
		entityengine->TimeScalingFactor *= 1.2;
//		std::cout << "Auto speed up\n";
//		std::cout << "Time scale:" << entityengine->TimeScalingFactor << "\n";
	}
	*/
//	renderengine->cam.yaw += DeltaTime * 30;
	//this->waterengine->Update(DeltaTime);
	if (guimanager->CurrentGui == GUIManager::GUIStates::GUIGame) {
		if (!GamePause) {
			entityengine->Update(DeltaTime);
		}
		audioengine->UpdateGame(*entityengine.get(), renderengine->cam);
		if (entityengine->FollowEntity != -1)
		{
			if (entityengine->WorldEntityMirror[entityengine->FollowEntity].ToRemove > 0)
			{
				entityengine->FollowEntity = -1;
			}
		}
		if (entityengine->FollowEntity != -1)
		{
			renderengine->cam.Position.x = entityengine->WorldEntityMirror[entityengine->FollowEntity].PositionX;
			renderengine->cam.Position.y = entityengine->WorldEntityMirror[entityengine->FollowEntity].PositionY;
			entityengine->PheremonePaintAffilation = entityengine->WorldEntityMirror[entityengine->FollowEntity].Affiliation;
		}
	}
	if (glfwWindowShouldClose(Window_Handle)) {
		Running = false;
	}
	//Update the 
	guimanager->Update();
	switch (guimanager->CurrentGui) {
	case GUIManager::GUIStates::GUIQuit:
		Running = false;
		break;
	case GUIManager::GUIStates::GUIGame:
			GUIGame& guigame = static_cast<GUIGame&>(guimanager->GetGUI(GUIManager::GUIStates::GUIGame));
			guigame.CurrentState.FollowEntId = entityengine->FollowEntity;
			if (entityengine->FollowEntity != -1)
			{
				guigame.CurrentState.followent = entityengine->WorldEntityMirror[entityengine->FollowEntity];

			}
			if (entityengine->FollowEntity != -1 && entityengine->WorldEntityMirror[entityengine->FollowEntity].Type == 0 || entityengine->WorldEntityMirror[entityengine->FollowEntity].Type == 1)
			{
				guigame.CurrentState.CurrentFaction = &entityengine->WorldFactionMirror[entityengine->WorldEntityMirror[entityengine->FollowEntity].Affiliation];
				entityengine->WorldFactionMirrorDirty[entityengine->WorldEntityMirror[entityengine->FollowEntity].Affiliation] = true;
			}
			else {
				guigame.CurrentState.CurrentFaction = nullptr;
			}
			if (DTStackpointer == 0)
			{
				guigame.CurrentState.FPS = int(1.0 / MeanDT);
			}
			guigame.CurrentState.TimeScalingFactor = entityengine->TimeScalingFactor;
			guigame.CurrentState.EntityCount = entityengine->ParticleCount;
		if (!GamePause) {
			std::array<int, 4> Accumulator = { 0 };
			std::array<int, WaterEngineGPU::FactionCount> AntAccumlator = { 0 };
			for (int i = 0; i < entityengine->ParticleCount; ++i)
			{
				if (entityengine->WorldEntityMirror[i].Alive == 1)
				{
					int type = int(entityengine->WorldEntityMirror[i].Type);
					if (type >= 0 && type < 4)
					{
						++Accumulator[type];
					}
					if (type == 0)
					{
						++AntAccumlator[int(entityengine->WorldEntityMirror[i].Affiliation)];
					}
				}
			}
			guigame.CurrentState.PredCount.push_back(Accumulator[3]);
			guigame.CurrentState.PreyCount.push_back(Accumulator[2]);
			for (int i = 0; i < guigame.CurrentState.AntFactionCount; ++i)
			{
				guigame.CurrentState.AntCount[i].push_back(AntAccumlator[i]);
			}
			break;
		}
	}
//	guigame.CurrentState.Ant0Count.push_back(Accumulator[0]);
}