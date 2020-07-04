#pragma once
#include "GUIBase.h"
#include "WaterEngineGPU.h"
#include <vector>
#include <array>
class GUIGame :
	public GUIBase
{
public:
	struct GameState {
		int FollowEntId = -1;
		GPUEntity followent;
		float FPS;
		float TimeScalingFactor;
		int EntityCount;
		std::vector<float> PreyCount;
		std::vector<float> PredCount;
		static constexpr int AntFactionCount = 1;
		std::array<std::vector<float>,AntFactionCount> AntCount;
		std::array<float*,2 + AntFactionCount> PlotArray;
		float MaxPopValue = 0;
		float MaxAntValue = 0;
		GPUFaction* CurrentFaction = nullptr;
			
//		float* PlotArray[3];
	} CurrentState;
	virtual void Update() override;
	virtual void Render() override;
};

