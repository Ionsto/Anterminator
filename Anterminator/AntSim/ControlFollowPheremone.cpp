#include "ControlFollowPheremone.h"
#include "Entity.h"
#include <glm/glm.hpp>
void ControlFollowPheremone::Update(World& world, Entity& entity,PheremoneGrid & Pheremone,float EntitySpecificMult)
{
	float h = 1;
	auto [ix,iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
	glm::vec2 meandir = glm::vec2(0,0);
	for (int x = -1; x < 2; ++x)
	{
		for (int y = -1; y < 2; ++y)
		{
			meandir += Pheremone.GetDirectionGrid(ix + x, iy + y) * Pheremone.GetStrengthGrid(ix+x,iy+y);
		}
	}
	meandir /= 9.0;
	entity.Heading += meandir * EntitySpecificMult;
}
