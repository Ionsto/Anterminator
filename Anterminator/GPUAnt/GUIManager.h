#pragma once
#include <vector>
#include "GUIBase.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <memory>
class GUIManager
{
	std::vector <std::unique_ptr<GUIBase>> guistack;
public:
	enum class GUIStates {
		GUIMainMenu,
		GUIGame,
		GUIQuit,
		GUIPauseMenu
	} CurrentGui = GUIStates::GUIMainMenu;
	std::map<std::pair<GUIStates, int>, GUIStates> TransitionMap;
	GUIManager(GLFWwindow* window);
	~GUIManager();
	void Update();
	void Render();
	GUIBase& GetGUI(GUIStates state) {
		return *guistack[int(state)].get();
	}
	void RegisterTransition(GUIStates from,int eventid,GUIStates to) {
		TransitionMap.insert({ std::pair(from, eventid), to });
	}
};

