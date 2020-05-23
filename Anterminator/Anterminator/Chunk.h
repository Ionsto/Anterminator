#pragma once
#include "SwapList.h"
#include "Entity.h"
class Chunk
{
	static constexpr int ChunkSize = 10;
	static constexpr int MaxEntities = 1000;
	SwapList<Entity, MaxEntities> EntityList;
	void Update();
};

