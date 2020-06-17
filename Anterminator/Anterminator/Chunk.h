#pragma once
#include "SwapList.h"
#include <array>
#include "Entity.h"
class Chunk
{
private:
	float X_Real = 0;
	float Y_Real = 0;
public:
	static constexpr float Size = 32;
	static constexpr int MaxEntities = 1000;
	int EntityCount = 0;
	std::array<int, MaxEntities> EntityIDList;
	float X = 0;
	float Y = 0;
	Chunk() {};
	Chunk(float x,float y);
	void Update();
};

