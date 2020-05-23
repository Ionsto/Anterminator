#pragma once
#include <array>
class Chunk
{
public:
	static constexpr int ChunkSize = 100;
	static constexpr int MaxEntities = 5000;
	int EntityCount = 0;
	std::array<int, MaxEntities> Entitiy;
};

