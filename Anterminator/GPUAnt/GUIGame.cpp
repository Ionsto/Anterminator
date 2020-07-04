#include "GUIGame.h"
#include "imgui.h"
#include "imgui_plot.h"
#include <GLFW/glfw3.h>
void GUIGame::Update()
{
	if (CurrentState.PredCount.size() > 0)
	{
		CurrentState.MaxPopValue = std::max(CurrentState.MaxPopValue,CurrentState.PredCount.back());
		CurrentState.MaxPopValue = std::max(CurrentState.MaxPopValue,CurrentState.PreyCount.back());
		for (int i = 0; i < CurrentState.AntFactionCount; ++i)
		{
			CurrentState.MaxAntValue = std::max(CurrentState.MaxAntValue,CurrentState.AntCount[i].back());
		}
	}
}

void GUIGame::Render()
{
	ImGui::Begin("Game stats");
	ImGui::BulletText("FPS: %4.2f", CurrentState.FPS);
	ImGui::BulletText("Entity count: %d", CurrentState.EntityCount);
	ImGui::BulletText("Time scale factor: %4.2f", CurrentState.TimeScalingFactor);
	ImGui::BulletText("Entity selected %d", CurrentState.FollowEntId);

	ImGui::End();
	if (CurrentState.FollowEntId != -1)
	{
		ImGui::Begin("Entity stats");
		auto e = CurrentState.followent;
		ImGui::BulletText("Info about entity: %d", CurrentState.FollowEntId);
		ImGui::BulletText("Alive: %f", e.Alive);
		ImGui::BulletText("Position: %f", e.PositionX, ", ", e.PositionY);
		ImGui::BulletText("Health: %f", e.Health);
		ImGui::BulletText("MaxHealth: %f", e.MaxHealth);
		ImGui::BulletText("Health %%: %f", 100 * e.Health / e.MaxHealth);
		ImGui::BulletText("Energy: %f", e.Energy);
		ImGui::BulletText("MaxEnergy: %f", e.MaxEnergy);
		ImGui::BulletText("Energy %%: %f", 100 * e.Energy / e.MaxEnergy);
		ImGui::BulletText("Age: %f", e.Age);
		ImGui::BulletText("Held food: %f", e.HeldFood);
		ImGui::BulletText("Time from home: %f", e.TimeFromHome);
		ImGui::BulletText("Size: %f", e.Size);
		ImGui::BulletText("Mass: %f", e.Mass);
		ImGui::End();
	}
	if (CurrentState.CurrentFaction != nullptr)
	{
		ImGui::Begin("Faction stat");
		ImGui::BulletText("Ant count: %d", CurrentState.CurrentFaction->AntCount);
		ImGui::BulletText("Nest count: %d", CurrentState.CurrentFaction->NestCount);
		ImGui::SliderFloat("Total ant spawn:", &CurrentState.CurrentFaction->AntSpawnRate, 0, 1);
		ImGui::SliderFloat("Total nest count:", &CurrentState.CurrentFaction->NestSpawnRate, 0, 1);
//		int AntCount = 0;
//		int NestCount = 0;
//		float TotalEnergy = 0;
//		float AntSpawnRate = 0;
//		float NestSpawnRate = 0;
		ImGui::End();
	}
	static ImU32 colors[3] = {
		ImColor(255, 0, 0),
		ImColor(0, 255, 0),
		ImColor(0, 0, 255)
	};
	CurrentState.PlotArray[0] = CurrentState.PredCount.data();
	CurrentState.PlotArray[1] = CurrentState.PreyCount.data();
	for (int i = 0; i < CurrentState.AntFactionCount; ++i) {
		CurrentState.PlotArray[2 + i] = CurrentState.AntCount[i].data();
	}
	//	CurrentState.PlotArray[2] = CurrentState.Ant0Count.data();
	{
		ImGui::Begin("Population");
		ImGui::PlotConfig conf;
		//	conf.values.xs = x_data; // this line is optional
		conf.scale.min = 0;
		conf.scale.max = CurrentState.MaxPopValue;
		conf.tooltip.show = true;
		conf.tooltip.format = "y=%.2f,y=%.2f,y=%.2f";
		conf.grid_x.show = false;
		conf.grid_y.show = false;
		conf.frame_size = ImVec2(400, 400);
		conf.line_thickness = 2.f;
		conf.values.count = CurrentState.PredCount.size();
		conf.values.ys_list = const_cast<const float**>(&CurrentState.PlotArray[0]);
		conf.values.ys_count = 2;
		conf.values.colors = colors;
		ImGui::Plot("Population", conf);
		ImGui::End();
	}

	{
		ImGui::Begin("Ant population");
		ImGui::BulletText("Current population: %d", int(CurrentState.AntCount[0].back()));
		ImGui::PlotConfig conf;
		//	conf.values.xs = x_data; // this line is optional
		conf.scale.min = 0;
		conf.scale.max = CurrentState.MaxAntValue;
		conf.tooltip.show = true;
//		conf.tooltip.format = "y=%.2f";
		conf.grid_x.show = false;
		conf.grid_y.show = false;
		conf.frame_size = ImVec2(400, 400);
		conf.line_thickness = 2.f;
		conf.values.ys = CurrentState.AntCount[0].data();
		conf.values.count = CurrentState.AntCount[0].size();
		conf.values.colors = colors;
		ImGui::Plot("Population", conf);
		ImGui::End();
	}
}
