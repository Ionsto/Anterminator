#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <memory>
#include "Shader.h"
#include "PerlinNoise.hpp"
#include "ShaderProgram.h"
#include "FastNoise.h"
//static constexpr int GPUSceneMaxInd = 5'000;
static constexpr int GPUSceneTypeCount = 5;
struct GPUScene {
	int ParticleCount = 0;
	std::array<int,GPUSceneTypeCount> TypeCount = { 0 };
//	std::array<std::array<int, GPUSceneMaxInd>, GPUSceneTypeCount> IndirectionId = { 0 };
};
static constexpr int MaxEntityPerChunk = 10;
struct GPUChunk {
	int EntityCount = 0;
	int EntityID[MaxEntityPerChunk];
};
struct GPUEntity {
	float PositionX = 0;
	float PositionY = 0;
	float PositionOldX = 0;
	float PositionOldY = 0;

	float AccelerationX = 0;
	float AccelerationY = 0;
	float Size = 1;
	float Alive = 1;

	float ColourR = 1;
	float ColourG = 1;
	float ColourB = 1;
	float ToRemove = 0;

	float Affiliation = -1;
	float Type = -1;
	float Mass = 1;
	float Age = 60;

	float HeadingX = 0;
	float HeadingY = 0;
	float Energy = 100;
	float HeldFood = 0;

	float TimeFromHome = 0;
	float Attacked = 0;
	float MaxEnergy = 100;
	float Health = 1;

	float MaxHealth = 1;
	float MaxSize;
	float PadB;
	float PadC;
};
struct GPUFaction {
	int AntCount = 0;
	int NestCount = 0;
	float TotalEnergy = 0;
	float AntSpawnRate = 0.2;
	float NestSpawnRate = 1;
};
struct GPUAudioEvent {
	float Type = 0;
	float Loudness = 0;
	float PositionX = 0;
	float PositionY = 0;
};
class WaterEngineGPU
{
public:
	float Time = 0;
	std::default_random_engine generator;
	std::uniform_real_distribution<float>  random_number;
	static const constexpr double DeltaTime = 0.02;
	static const constexpr float WorldSize = 2048;
	static const constexpr float ChunkSize = 8;
	static const constexpr int ChunkCount = (2 * WorldSize) / ChunkSize;
	int AIUpdateTick = 0;
	int FlowCount = 0;
	int FollowEntity = 0;
	GLFWwindow* Window_Handle;
	Shader Compute_CollisionUpdate;
	Shader Compute_EntityAIUpdate;
	Shader Compute_EntityUpdate;
	Shader Compute_ChunkUpdate;
	Shader Compute_ChunkReset;
	Shader Compute_PheremoneUpdate;
	Shader Compute_PheremonePaint;
	Shader Compute_RemoveDead;
	Shader Compute_AudioEvents;
	ShaderProgram Program_CollisionUpdate;
	ShaderProgram Program_EntityAIUpdate;
	ShaderProgram Program_EntityUpdate;
	ShaderProgram Program_ChunkUpdate;
	ShaderProgram Program_ChunkReset;
	ShaderProgram Program_PheremoneUpdate;
	ShaderProgram Program_PheremonePaint;
	ShaderProgram Program_RemoveDead;
	ShaderProgram Program_AudioEvents;
	GLuint EntityBuffer;
	GLuint RenderPositionBuffer;
	GLuint RenderColourBuffer;
	GLuint RenderSizeBuffer;
	GLuint SceneBuffer;
	GLuint ChunkBuffer;
	GLuint SceneOutputBuffer;
	GLuint EntityOutputBuffer;
	GLuint FactionOutputBuffer;
	GLuint FactionBuffer;
	GLuint AudioEventBuffer;
	GLuint AudioOutputBuffer;

	GLuint UniformEntityEntityCount;       
	GLuint UniformEntityWorldSize;       
	GLuint UniformEntityRandomTimeSeed;
	GLuint UniformEntityAIEntityCount;       
	GLuint UniformEntityAIWorldSize;       
	GLuint UniformEntityAIRandomTimeSeed;
	GLuint UniformEntityExecutionType;
	GLuint UniformCollisionRandomTimeSeed;
	GLuint UniformCollisionCollsionMap;
	GLuint UniformAudioRandomTimeSeed;
	GLuint UniformTextureHome;
	GLuint UniformTextureFood;
	GLuint UniformTextureDead;
	GLuint UniformTextureAttack;
	GLuint UniformUpdatePheremone;
	GLuint UniformPHUpdateDecayRate;
	GLuint UniformPHUpdateDiffusionConst;
	GLuint UniformPaintAffiliation;

	GLuint UniformPhermonePaint;
	GLuint TexturePheremonePaint;
	static constexpr int PhermoneResolution = 4;
	std::array<GLuint,4> TexturePheremone;
	std::array<float, 4> PhermoneDecayRate = {0.0001,0.0001,0.001,0.02};
	std::array<float, 4> PhermoneDiffusion = {0.001,0.001,0.01,4};
	static constexpr int PheremoneSize =  2 * WorldSize/ PhermoneResolution;
	std::vector<float> PheremonePainter = std::vector<float>(PheremoneSize * PheremoneSize * 4);
	bool PheremonePainterDirty = false;
	int PheremonePainterDirtyCounter = 0;
	int PheremonePaintAffilation = 0;
	
	GLuint TextureWorldMap;
	static constexpr int WorldTextureResolution = 4;
	static constexpr int WorldTextureSize =  2 * WorldSize/ WorldTextureResolution;
	std::vector<float> WorldNoiseMask = std::vector<float>(WorldTextureSize * WorldTextureSize);

	float WrapValue(float v)
	{
		if(v < -WorldSize)
		{
			v += 2 * WorldSize;
		}
		if(v >= WorldSize)
		{
			v -= 2 * WorldSize;
		}
		return v;
	}
	int GetPheremoneIndex(float x_a,float y_a) {
		x_a = WrapValue(x_a);
		y_a = WrapValue(y_a);
		int x = (x_a + WorldSize) / PhermoneResolution;
		int y = (y_a + WorldSize) / PhermoneResolution;
		return (x + (y * PheremoneSize))*4;
	}

	static constexpr int FactionCount = 4;

	float TimeScalingFactor = 1;
	float DtAccumulator = 0;
	static constexpr const int MaxParticleCount = 20'000;
	//How often we pull gpu data onto the mirror in ram
	int EntityUpdateCounter = 0;
	int AudioUpdateCounter = 0;
	std::vector<GPUEntity> WorldEntityMirror = std::vector<GPUEntity>(MaxParticleCount);
	std::vector<GPUFaction> WorldFactionMirror = std::vector<GPUFaction>(FactionCount);
	std::vector<bool> WorldFactionMirrorDirty = std::vector<bool>(FactionCount);
	static constexpr const int MaxAudioEvents = 5000;
	std::vector<GPUAudioEvent> GPUAudioEventList = std::vector<GPUAudioEvent>(MaxAudioEvents);
	int ParticleCount = 0;

	std::unique_ptr<GPUScene> GPUSceneTransferBuffer;
	siv::PerlinNoise PerlinGenerator;
	FastNoise noisegen;

	GPUScene GPUscene;
	WaterEngineGPU();
	void Init(GLFWwindow * handle);
	~WaterEngineGPU();
	void Update(float realdt);
	void UpdateStep(bool removedead = true,bool chunkupdate = true, bool entityupdate = true);
	inline float WrapDistance(float a, float b)
	{
		float dist = a - b;
		if (abs(dist) > WorldSize)
		{
			dist = (abs(dist) - (WorldSize*2)) * copysignf(1,dist);
		}
		if (abs(dist) > WorldSize)
		{
			//dist = WorldSize - dist;
			//dist = std::copysignf(abs(dist) - WorldSize/2,-dist);
		//	dist = std::copysignf(WorldSize - abs(dist),-dist);
		}
		return dist;
	}
	int ResetWorld();
	int InitWorld();
	int PrintEntityInfo(float x, float y) {
//		auto& c = Chunks.GetChunkPos(x, y);
		for (int i = 0; i < ParticleCount; ++i)
		{
			auto& e = WorldEntityMirror[i];
//			auto dist = glm::vec2(WrapDistance(pos.x , e.Position.x), WrapDistance(pos.y , e.Position.y));
			auto dist = glm::vec2(WrapDistance(x,e.PositionX),WrapDistance(y,e.PositionY));
			if (glm::length(dist) < e.Size) {
				PrintEntityInfoId(i);
				return i;
			}
		}
		return -1;
	}
	void PrintEntityInfoId(int i) {
		auto& e = WorldEntityMirror[i];
		std::cout << "-----------------------------\n";
		std::cout << "Info about entity:" << i << "\n";
		std::cout << "Alive:" << e.Alive << "\n";
		std::cout << "Position:" << e.PositionX<<", " << e.PositionY << "\n";
		std::cout << "Health:" << e.Health << "\n";
		std::cout << "MaxHealth:" << e.MaxHealth << "\n";
		std::cout << "Health %:" << 100 *e.Health/e.MaxHealth << "%\n";
		std::cout << "Energy:" << e.Energy << "\n";
		std::cout << "MaxEnergy:" << e.MaxEnergy << "\n";
		std::cout << "Energy %:" << 100 *e.Energy/e.MaxEnergy  << "%\n";
		std::cout << "Age:" << e.Age << "\n";
		std::cout << "Held food:" << e.HeldFood << "\n";
		std::cout << "Time from home:" << e.TimeFromHome << "\n";
	}

};

