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
	static constexpr int GridSize = 256*2;
	float MaxStrength = 100;
	float DecayRate = 1;
	float DiffusionConstant = 0.01;
	float MaxDiffusionConstant = 0.5;

	int DiffuseTime = rand()%100;
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
	/*Pheremone& GetPheremone(float x, float y, int sw = -1)
	{
		if (sw == -1)
		{
			sw = SwapBuffer;
		}
		int ix = (1.0 + (x / WorldSize)) * (GridSize/2.0);
		int iy = (1.0 + (y / WorldSize)) * (GridSize/2.0);
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		Pheremone p;
		p.Direction = &cached_data[sw].Direction[(ix * GridSize) + iy];
		p.Strength = &cached_data[sw].Strength[(ix * GridSize) + iy];
		return p;
	}
	Pheremone& GetPheremoneGrid(int ix, int iy,int sw = -1)
	{
		if (sw == -1)
		{
			sw = SwapBuffer;
		}
		ix = std::clamp(ix, 0, GridSize - 1);
		iy = std::clamp(iy, 0, GridSize - 1);
		Pheremone p;
		p.Direction = &cached_data[sw].Direction[(ix * GridSize) + iy];
		p.Strength = &cached_data[sw].Strength[(ix * GridSize) + iy];
		return p;
		//return raw_data[(sw * GridSize*GridSize) + (ix * GridSize) + iy];
	}
	Pheremone& GetPheremoneGridFast(int ix, int iy)
	{
		Pheremone p;
		p.Direction = &cached_data[SwapBuffer].Direction[(ix * GridSize) + iy];
		p.Strength = &cached_data[SwapBuffer].Strength[(ix * GridSize) + iy];
		return p;
	}*/
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
	glm::vec2& GetDirectionFast(int ix, int iy)
	{
		return cached_data[SwapBuffer].Direction[(ix * GridSize) + iy];
	}
	float& GetStrengthFast(int ix, int iy)
	{
		return cached_data[SwapBuffer].Strength[(ix * GridSize) + iy];
	}
	inline int fast_rand(void) {
		g_seed = (214013 * g_seed + 2531011);
		return (g_seed >> 16) & 0x7FFF;
	}
	void Update(float DeltaTime) {
		if (DiffuseTime++ > 100+ rand() % 5)
		{
		
#pragma omp parallel for
			for (int i = 0; i < cached_data[SwapBuffer].Direction.size(); ++i)
			{
				auto& Strength = cached_data[SwapBuffer].Strength[i];
				auto& Direction = cached_data[SwapBuffer].Direction[i];
				Strength = std::clamp(Strength - (DecayRate * DeltaTime* static_cast<float>(DiffuseTime)), 0.0f, MaxStrength);
				//v.Direction += (glm::vec2(fast_rand() % 11 / 10, fast_rand() % 11 / 10) - 0.5f) * 0.1f;
				if (Strength < 0.1)
				{
					Direction.x = 0;
					Direction.y = 0;
					Strength = 0;
				}
			}
			float MeshDiffusionNumber = DiffusionConstant * (DeltaTime * static_cast<float>(DiffuseTime)) / (WorldSize / GridSize);
			MeshDiffusionNumber = std::min(MeshDiffusionNumber, MaxDiffusionConstant);
			float Crecip = 1.0 / (1.0f + (MeshDiffusionNumber * 4.0f));
#pragma omp parallel for
			for (int x = 1; x < GridSize-1; ++x)
			{
//#pragma omp for simd
#pragma omp simd 
				for (int y = 1; y < GridSize-1; ++y)
				{
					cached_data[SwapBuffer^1].Direction[(x * GridSize) + y] =
						(GetDirectionFast(x, y) +
							(MeshDiffusionNumber * (
								GetDirectionFast(x - 1, y)
								+ GetDirectionFast(x + 1, y)
								+ GetDirectionFast(x, y - 1)
								+ GetDirectionFast(x, y + 1)))) * Crecip;
					cached_data[SwapBuffer^1].Strength[(x * GridSize) + y] = 
							(GetStrengthFast(x, y) +
							(MeshDiffusionNumber * (
								GetStrengthFast(x - 1, y)
								+ GetStrengthFast(x + 1, y)
								+ GetStrengthFast(x, y - 1)
								+ GetStrengthFast(x, y + 1)))) * Crecip;
				}
			}
			SwapBuffer ^= 1;
/*#pragma omp parallel for
			for (int x = 1; x < GridSize-1; ++x)
			{
				for (int y = 1; y < GridSize-1; ++y)
				{
					raw_data[y + (x * GridSize)].Direction = Backbuffer[y + (x * GridSize)];
					raw_data[y + (x * GridSize)].Strength = Backbufferstrength[y + (x * GridSize)];
				}
			}
			*/

			for (int v = 0; v < GridSize; ++v)
			{
				GetStrengthGrid(v,0) = 0;
				GetStrengthGrid(v,(GridSize-1)) = 0;
				GetStrengthGrid(0,v) = 0;
				GetStrengthGrid((GridSize-1),v) = 0;
			}
			DiffuseTime = 0;
		}
	}
};
