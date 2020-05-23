#pragma once
#include "PheremoneGrid.h"
class World;
class Entity;
class ControlPlacePheremone
{
	float BasePlaceParamater = 1;
	float ModifiedPlace = 1;
public:
	float SpreadSize = 1;
	void Update(World& world, Entity& entity,PheremoneGrid & Pheremone,float s,float v);
};

