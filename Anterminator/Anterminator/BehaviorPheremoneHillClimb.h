#pragma once
#include "Entity.h"
#include "World.h"
#include "PheremoneGrid.h"
namespace BehaviorPheremoneHillClimb
{
	void Update(Entity& e, PheremoneGrid& Pheremone, float EntitySpecificMult) {
		float h = 1;
		//auto [ix, iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
		glm::vec2 meandir = glm::vec2(0, 0);
		float strengthacc = 0;
		int halfspan = 3;
		for (int x = -halfspan; x <= halfspan; ++x)
		{
			for (int y = -halfspan; y <= halfspan; ++y)
			{
				float strength = Pheremone.GetPheremone(e.Position.x + x, e.Position.y + y).Strength;
				meandir += glm::vec2(x, y) * strength * strength;
				//meandir += Pheremone.GetPheremoneGrid(ix + x, iy + y).Direction * Pheremone.GetPheremoneGrid(ix + x, iy + y).Strength;
			}
		}
		meandir /= ((halfspan * 2) + 1.0f) * ((halfspan * 2) + 1.0f);
		e.Heading += meandir * EntitySpecificMult;
	}
}
