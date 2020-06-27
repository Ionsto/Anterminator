#include "GUIManager.h"
#include "GUIGame.h"
#include "GUIMainMenu.h"
#include "GUIPauseMenu.h"
GUIManager::GUIManager(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
	CurrentGui == GUIStates::GUIMainMenu;

	guistack.push_back(std::make_unique<GUIMainMenu>());
	RegisterTransition(GUIStates::GUIMainMenu, 0, GUIStates::GUIGame);
	RegisterTransition(GUIStates::GUIMainMenu, 1, GUIStates::GUIQuit);
	guistack.push_back(std::make_unique<GUIGame>());
	RegisterTransition(GUIStates::GUIGame, 0, GUIStates::GUIMainMenu);
//	RegisterTransition(GUIStates::GUIGame, 1, GUIStates::GUIQuit);
	guistack.push_back(std::make_unique<GUIBase>());
	guistack.push_back(std::make_unique<GUIPauseMenu>());
	RegisterTransition(GUIStates::GUIPauseMenu, 0, GUIStates::GUIMainMenu);
	RegisterTransition(GUIStates::GUIPauseMenu, 1, GUIStates::GUIGame);
}
GUIManager::~GUIManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void GUIManager::Update()
{
	if (int(CurrentGui) >= 0 && int(CurrentGui) < guistack.size())
	{
		int id = int(CurrentGui);
		guistack[id]->Update();
		if (guistack[id]->EventRaised)
		{
			auto key = std::pair<GUIStates, int>(CurrentGui, guistack[id]->EventID);
			if (this->TransitionMap.contains(key)) {
				CurrentGui = TransitionMap[key];
			}
			else
			{
				std::cout << "Unknown transition\n";
			}
		}
	}
}
void GUIManager::Render()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (int(CurrentGui) >= 0 && int(CurrentGui) < guistack.size())
	{
		guistack[int(CurrentGui)]->Render();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
