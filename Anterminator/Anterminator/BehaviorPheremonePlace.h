#pragma once
#include "PheremoneGrid.h"
namespace BehaviorPheremonePlace {
	void Update(Entity& entity, PheremoneGrid& Pheremone, float SpreadSize, float Strength, glm::vec2 speed)
	{
		int x = 0;
		int y = 0;
		for (int x = -SpreadSize + 1; x < SpreadSize; ++x)
		{

			for (int y = -SpreadSize + 1; y < SpreadSize; ++y)
			{
				//auto [ix,iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
				auto& ph = Pheremone.GetPheremone(entity.Position.x + x, entity.Position.y + y);
				auto& pDirection = ph.Direction;
				float& pStrength = ph.Strength;
//				float Delta = (Strength / SpreadSize) / (1 + (std::sqrtf(x * x + y * y)));
				float Delta = (Strength) / (1 + (std::sqrtf(x * x + y * y)));
				if (pStrength + Delta != 0)
				{
					pDirection = ((pDirection * pStrength) + (speed * Delta)) / (pStrength + Delta);
					pStrength += Delta;
					Pheremone.GetChunk(entity.Position.x + x, entity.Position.y + y).Active = true;
					Pheremone.GetChunk(entity.Position.x + x, entity.Position.y + y).ZeroValue = false;
				}
			}
		}
	}
	void Update(Entity& entity, PheremoneGrid& Pheremone, float SpreadSize, float Strength, float Direction)
	{
		auto speed = ((entity.PositionOld - entity.Position) / World::DeltaTime) * Direction;
		Update(entity, Pheremone, SpreadSize, Strength, speed);
	}
}
