#include "ControlPlacePheremone.h"
#include "World.h"
#include "Entity.h"
#include <glm/glm.hpp>
void ControlPlacePheremone::Update(World& world, Entity& entity,PheremoneGrid & Pheremone,float s,float v)
{
	for (float x = -SpreadSize+1; x < SpreadSize; ++x)
	{

		for (float y = -SpreadSize+1; y < SpreadSize; ++y)
		{
			auto [ix,iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
			auto& pDirection = Pheremone.GetDirectionGrid(ix + x, iy + y);
			float& pStrength = Pheremone.GetStrengthGrid(ix + x, iy + y);
			float Delta = (s / SpreadSize) / (1+(std::sqrtf(x*x + y*y)));
			auto speed = ((entity.OldPosition - entity.Position) / world.DeltaTime) * v;
			pDirection = ((pDirection * pStrength) + (speed * Delta)) / (pStrength + Delta);
			pStrength += Delta;
		}
	}
	//float Delta = BasePlaceParamater * ModifiedPlace * s;
	//float Sd = ((pval.Value * pval.Strength) + (v * Delta))/(pval.Strength + Delta);
	//pval.Value = Sd;
	//pval.Strength = (pval.Strength + Delta);
	
}
