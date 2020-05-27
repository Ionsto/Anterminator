#include "ControlHillClimbPheremone.h"
#include "Entity.h"
#include <glm/glm.hpp>
void ControlHillClimbPheremone::Update(World& world, Entity& entity, PheremoneGrid& Pheremone, float EntitySpecificMult)
{
	float h = 1;
	auto [ix, iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
	glm::vec2 meandir = glm::vec2(0, 0);
	float strengthacc = 0;
	int halfspan = 3;
	for (int x = -halfspan; x <= halfspan; ++x)
	{
#pragma omp simd
		for (int y = -halfspan; y <= halfspan; ++y)
		{
			float strength = Pheremone.GetStrengthGrid(ix + x, iy + y);
			meandir += glm::vec2(x, y) * strength * strength;
			//meandir += Pheremone.GetPheremoneGrid(ix + x, iy + y).Direction * Pheremone.GetPheremoneGrid(ix + x, iy + y).Strength;
		}
	}
	meandir /= ((halfspan*2)+1.0f) * ((halfspan*2)+1.0f);
	entity.Heading += meandir * EntitySpecificMult;
}
