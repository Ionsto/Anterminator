#pragma once
#include "PheremoneGrid.h"
class World;
class Entity;
class ControlFollowPheremone
{
	float SpeedSetting = 1;
	float Direction = 1;
public:
	void Update(World& world, Entity& entity,PheremoneGrid & Pheremone,float EntitySpecificMult);
};

