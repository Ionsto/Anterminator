#pragma once
#include <array>
#include "Chunk.h"
#include "ChunkMap.h"
#include "AntColonyController.h"
#include <random>
class World
{
public:
	std::default_random_engine generator;
	std::uniform_real_distribution<float>  random_number;
	static constexpr float DeltaTime = 0.01;
	static constexpr float WorldSize = 4096;
	static constexpr int ChunkCount = static_cast<int>(WorldSize/Chunk::Size);
//World extends from -worldsize to +worldsize
//	std::array<Chunk, WorldSize* WorldSize> ChunkArray;
	float DtAccumulator = 0;
	float TimeScalingFactor = 1.0/(1<<2);
	ChunkMap<ChunkCount> Chunks;
	static constexpr int MaxEntities = 100'000;
	SwapList<Entity, MaxEntities> EntityList;
	static constexpr int AntFactionCount = 100;
	std::array<AntColonyController, AntFactionCount> FactionArray;
	World();
	void Update(float realdt);
	bool AddEntity(Entity&& entity);
	void UpdateColonies();
	void UpdateSpacialHash();
	void UpdateCollisionEntities(Entity& e, Entity& ej);
	void UpdateCollisionChunk(int x, int y, int xn, int yn);
	void UpdateCollisionSingleChunk(int x, int y);
	void UpdateCollisionChunk(int i);
	void UpdateCollision();
	void UpdateEntities();
	void UpdateDeadEntities();
	Entity PrintEntityInfo(float x, float y);
	float PreySpawnCount = 0;
	void AddPrey();
	inline float WrapDistance(float a, float b)
	{
		float dist = a - b;
		if (abs(dist) > WorldSize/2)
		{
			//dist = WorldSize - dist;
			//dist = std::copysignf(abs(dist) - WorldSize/2,-dist);
			dist = std::copysignf(WorldSize - abs(dist),-dist);
		}
		return dist;
	}
	float WrapOther(float a, float b)
	{
		float dist = a - b;
		if (abs(dist) >= WorldSize/2)
		{
			dist = std::copysignf(abs(dist) - (WorldSize),-dist);
		}
		return dist;
	}
};

