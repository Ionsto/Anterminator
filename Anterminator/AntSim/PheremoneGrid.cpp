#include "PheremoneGrid.h"
void PheremoneGrid::Update(float DeltaTime) {
	if (DiffuseTime++ > DiffuseTimeMax+ rand() % 5)
	{
	
//#pragma omp parallel for
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
		float MeshDiffusionNumber = DiffusionConstant * (DeltaTime * static_cast<float>(DiffuseTime)) / Size;
		MeshDiffusionNumber = std::min(MeshDiffusionNumber, MaxDiffusionConstant);
		float Crecip = 1.0 / (1.0f + (MeshDiffusionNumber * 4.0f));
//#pragma omp parallel for
		for (int x = 1; x < GridSize-1; ++x)
		{
//#pragma omp simd 
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
			GetStrengthFast(v,0) = 0;
			GetStrengthFast(v,(GridSize-1)) = 0;
			GetStrengthFast(0,v) = 0;
			GetStrengthFast((GridSize-1),v) = 0;
		}
		DiffuseTime = 0;
	}
}
