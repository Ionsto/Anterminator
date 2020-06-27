#include "GUIMainMenu.h"
#include "imgui.h"
void GUIMainMenu::Update()
{
}

void GUIMainMenu::Render()
{
	ImGui::Begin("Main menu");
	if (ImGui::Button("Start")) {
		RaiseEvent(0);
	}
	if (ImGui::Button("Quit")) {
		 RaiseEvent(1);
	}
	ImGui::End();
}
