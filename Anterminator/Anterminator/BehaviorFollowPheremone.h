#pragma once
#include "Entity.h"
#include "World.h"
#include "PheremoneGrid.h"
namespace BehaviorFollowPheremone
{
	void Update(Entity & e,PheremoneGrid & Pheremone,float EntitySpecificMult) {
		float h = 1;
		//auto [ix,iy] = Pheremone.GetGridPos(entity.Position.x, entity.Position.y);
		glm::vec2 meandir = glm::vec2(0,0);
		int SampleSize = 2;
		float SampleWidth = 5;
		float Res = SampleWidth / SampleSize;
		for (int x = -SampleSize; x <= SampleSize; ++x)
		{
			for (int y = -SampleSize; y <= SampleSize; ++y)
			{
				auto& p = Pheremone.GetPheremone(e.Position.x + (x*Res), e.Position.y + (y*Res));
				meandir += p.Direction * p.Strength;
			}
		}
		meandir /= 9.0;
		e.Heading += meandir * EntitySpecificMult;
	}

}
