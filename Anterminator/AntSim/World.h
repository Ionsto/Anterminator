#pragma once
#include "ChunkArray.h"
#include "Entity.h"
#include "ColonyController.h"
#include "ControlRandomWalk.h"
#include <array>
#include <vector>
#include <random>
#include <chrono>
#include "SwapList.h"
class World
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> Timer;
	float DtAccumulator;
	int GraphSampleCounter = 0;
public:
	std::default_random_engine generator;
	std::uniform_real_distribution<float>  random_number;
	PheremoneGrid FoodScent;
	ControlRandomWalk NPCWalking;
	ControlPlacePheremone FoodScentPlace;
	float ElapsedTime = 0;
	float TimeScalingFactor = 1;
	float TimeSpacialHash = 0;
	float TimeCollisions = 0;
	float TimeUpdateEntity = 0;
	float TimeUpdateColonies = 0;
	static constexpr float DeltaTime = 0.01;
	static constexpr int WorldSize = 1000;
	static constexpr int MaxEntityCount = 100000;
	SwapList<Entity, MaxEntityCount> EntityList;
	static constexpr int MaxFoodCount = 10;
	SwapList<Entity, MaxFoodCount> FoodList;
	//std::vector<Chunk> ChunkArray(static_cast<int>((WorldSize/Chunk::ChunkSize)*(WorldSize/Chunk::ChunkSize)));
	ChunkArray<WorldSize> Chunks;
	int AntCounter = 0;
	float FoodCounter = 0;
	float FoodDecrement = 0;
	static constexpr int AntColonyMax = 5;
	int AntColonyCount = AntColonyMax;
	std::array<ColonyController, AntColonyMax> ColonyArray;
	std::array<int, AntColonyMax + 1> AffiliationCounter = { 0 };
	std::vector<std::vector<float>> AntCountHistory = std::vector<std::vector<float>>(World::AntColonyMax);
	World();
	void Update(float realdt);
	void UpdateColonies();
	void UpdateEntities();
	void UpdateCollisionEntities(Entity& e, Entity& ej);
	void UpdateCollisionSingleChunk(int cx, int cy);
	void UpdateCollisionChunk(int cx, int cy,int nx,int ny);
	void UpdateCollisions();
	void UpdateDeadEntities();
	void UpdateSpacialHash();
	bool AddEntity(Entity e);
	void PrintEntityInfo(float x, float y);
};

