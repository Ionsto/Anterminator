#include "GUIPauseMenu.h"
#include "imgui.h"
void GUIPauseMenu::Update()
{
}

void GUIPauseMenu::Render()
{
	ImGui::Begin("Pause menu");
	if (ImGui::Button("Resume")) {
		RaiseEvent(0);
	}
	if (ImGui::Button("Main menu")) {
		 RaiseEvent(1);
	}
	ImGui::End();
}
