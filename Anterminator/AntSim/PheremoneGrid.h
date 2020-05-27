#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <array>
static int g_seed = 1234213125345436254 % 329801;
struct Pheremone {
	glm::vec2 * Direction;
	float * Strength;
};
struct PheremoneCached {
	static constexpr int GridSize = 256*2;
	std::vector<glm::vec2> Direction = std::vector<glm::vec2>(GridSize * GridSize);
	std::vector<float> Strength = std::vector<float>(GridSize * GridSize);
};
struct PheremoneGrid {
	static constexpr int WorldSize = 1000;
	static constexpr int GridSize = PheremoneCached::GridSize;
	static constexpr float Size = static_cast<float>(WorldSize)/static_cast<float>(GridSize);
	float MaxStrength = 100;
	float DecayRate = 1;
	float DiffusionConstant = 0.01;
	float MaxDiffusionConstant = 100;

	int DiffuseTime = rand()%100;
	int DiffuseTimeMax = 50;
	//std::vector<Pheremone> raw_data = std::vector<Pheremone>(GridSize * GridSize);
//	std::array<Pheremone, GridSize* GridSize> raw_data = {0};
	int SwapBuffer = 0;
	std::array<PheremoneCached, 2> cached_data;
	//std::vector<Pheremone> raw_data = std::vector<Pheremone>(2*GridSize * GridSize);
	//std::vector<glm::vec2> Backbuffer = std::vector<glm::vec2>(GridSize * GridSize);
	//std::vector<float> Backbufferstrength = std::vector<float>(GridSize * GridSize);
	PheremoneGrid() {
//		std::fill(raw_data.begin(),raw_data.end(), 0);
	}
	std::tuple<int,int> GetGridPos(float x, float y)
	{
		int ix = (1.0 + (x / WorldSize)) * (GridSize/2.0);
		int iy = (1.0 + (y / WorldSize)) * (GridSize/2.0);
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		return { ix,iy };
	}
	glm::vec2& GetDirection(float x, float y,int sw = -1)
	{
		if (sw == -1)
		{
			sw = SwapBuffer;
		}
		int ix = (1.0 + (x / WorldSize)) * (GridSize/2.0);
		int iy = (1.0 + (y / WorldSize)) * (GridSize/2.0);
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		return cached_data[SwapBuffer].Direction[(ix * GridSize) + iy];
	}
	float& GetStrength(float x, float y,int sw = -1)
	{
		if (sw == -1)
		{
			sw = SwapBuffer;
		}
		int ix = (1.0 + (x / WorldSize)) * (GridSize/2.0);
		int iy = (1.0 + (y / WorldSize)) * (GridSize/2.0);
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		return cached_data[SwapBuffer].Strength[(ix * GridSize) + iy];
	}
	glm::vec2& GetDirectionGrid(int ix, int iy)
	{
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		return cached_data[SwapBuffer].Direction[(ix * GridSize) + iy];
	}
	float& GetStrengthGrid(int ix, int iy)
	{
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		return cached_data[SwapBuffer].Strength[(ix * GridSize) + iy];
	}
	inline glm::vec2& GetDirectionFast(int ix, int iy)
	{
		return cached_data[SwapBuffer].Direction[(ix * GridSize) + iy];
	}
	inline float& GetStrengthFast(int ix, int iy)
	{
		return cached_data[SwapBuffer].Strength[(ix * GridSize) + iy];
	}
	inline int fast_rand(void) {
		g_seed = (214013 * g_seed + 2531011);
		return (g_seed >> 16) & 0x7FFF;
	}
	void Update(float DeltaTime);
};
