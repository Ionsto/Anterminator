#include "WaterEngineGPU.h"
#include <iostream>
#include <chrono>
#include "TinyPngOut.hpp"
#include "PerlinNoise.hpp"
#include <ostream>
#include <fstream>


WaterEngineGPU::WaterEngineGPU():random_number(-1,1), PerlinGenerator(39123)

{
	generator.seed(time(NULL));
	GPUSceneTransferBuffer = std::make_unique<GPUScene>();
}
void WaterEngineGPU::Init(GLFWwindow * handle)
{
	Window_Handle = handle;
	Compute_EntityUpdate.Init("entityupdate.comp", GL_COMPUTE_SHADER);
	Compute_EntityAIUpdate.Init("entityaiupdate.comp", GL_COMPUTE_SHADER);
	Compute_ChunkUpdate.Init("chunkupdate.comp", GL_COMPUTE_SHADER);
	Compute_ChunkReset.Init("chunkreset.comp", GL_COMPUTE_SHADER);
	Compute_PheremoneUpdate.Init("pheremoneupdate.comp", GL_COMPUTE_SHADER);
	Compute_PheremonePaint.Init("pheremonepaint.comp", GL_COMPUTE_SHADER);
	Compute_RemoveDead.Init("removedead.comp", GL_COMPUTE_SHADER);
	Compute_CollisionUpdate.Init("collisionupdate.comp", GL_COMPUTE_SHADER);
	Program_EntityUpdate.CreateProgram();
	Program_EntityUpdate.AddShader(Compute_EntityUpdate);
	Program_EntityUpdate.LinkProgram();

	Program_EntityAIUpdate.CreateProgram();
	Program_EntityAIUpdate.AddShader(Compute_EntityAIUpdate);
	Program_EntityAIUpdate.LinkProgram();

	Program_CollisionUpdate.CreateProgram();
	Program_CollisionUpdate.AddShader(Compute_CollisionUpdate);
	Program_CollisionUpdate.LinkProgram();

	Program_ChunkUpdate.CreateProgram();
	Program_ChunkUpdate.AddShader(Compute_ChunkUpdate);
	Program_ChunkUpdate.LinkProgram();

	Program_ChunkReset.CreateProgram();
	Program_ChunkReset.AddShader(Compute_ChunkReset);
	Program_ChunkReset.LinkProgram();

	Program_PheremoneUpdate.CreateProgram();
	Program_PheremoneUpdate.AddShader(Compute_PheremoneUpdate);
	Program_PheremoneUpdate.LinkProgram();

	Program_RemoveDead.CreateProgram();
	Program_RemoveDead.AddShader(Compute_RemoveDead);
	Program_RemoveDead.LinkProgram();

	Program_PheremonePaint.CreateProgram();
	Program_PheremonePaint.AddShader(Compute_PheremonePaint);
	Program_PheremonePaint.LinkProgram();

	Program_ChunkUpdate.UseProgram();
	Program_EntityUpdate.UseProgram();
	glGenBuffers(1, &EntityBuffer);
	glGenBuffers(1, &RenderPositionBuffer);
	glGenBuffers(1, &RenderColourBuffer);
	glGenBuffers(1, &RenderSizeBuffer);
	glGenBuffers(1, &SceneBuffer);
	glGenBuffers(1, &ChunkBuffer);
	glGenBuffers(1, &FactionBuffer);
	glGenBuffers(1, &SceneOutputBuffer);
	glGenBuffers(1, &EntityOutputBuffer);
	glGenBuffers(1, &FactionOutputBuffer);
	GPUscene.ParticleCount = 0;

	UniformEntityEntityCount = glGetUniformLocation(Program_EntityUpdate.GetProgram(), "EntityCount");
	UniformEntityWorldSize = glGetUniformLocation(Program_EntityUpdate.GetProgram(), "WorldSize");
	UniformEntityRandomTimeSeed = glGetUniformLocation(Program_EntityUpdate.GetProgram(), "RandomTimeSeed");
	UniformEntityExecutionType = glGetUniformLocation(Program_EntityUpdate.GetProgram(), "ExecutionType");

	UniformEntityAIEntityCount = glGetUniformLocation(Program_EntityAIUpdate.GetProgram(), "EntityCount");
	UniformEntityAIWorldSize = glGetUniformLocation(Program_EntityAIUpdate.GetProgram(), "WrldSize");
	UniformEntityAIRandomTimeSeed = glGetUniformLocation(Program_EntityAIUpdate.GetProgram(), "RandomTimeSeed");

	UniformCollisionRandomTimeSeed = glGetUniformLocation(Program_CollisionUpdate.GetProgram(), "RandomTimeSeed");
	UniformUpdatePheremone = glGetUniformLocation(Program_PheremoneUpdate.GetProgram(), "Pheremone");
	UniformPHUpdateDecayRate = glGetUniformLocation(Program_PheremoneUpdate.GetProgram(), "DecayConstant");
	UniformPHUpdateDiffusionConst = glGetUniformLocation(Program_PheremoneUpdate.GetProgram(), "DiffusionConstant");
	UniformPhermonePaint = glGetUniformLocation(Program_PheremonePaint.GetProgram(), "Pheremone");
	UniformPaintAffiliation = glGetUniformLocation(Program_PheremonePaint.GetProgram(), "Affiliation");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EntityBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUEntity) * MaxParticleCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderPositionBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * MaxParticleCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderColourBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * MaxParticleCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RenderSizeBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 1 * MaxParticleCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUScene),&GPUscene, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_COPY_READ_BUFFER, SceneOutputBuffer);
	glBufferStorage(GL_COPY_READ_BUFFER, sizeof(GPUScene),&GPUscene, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ChunkBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUChunk) * ChunkCount * ChunkCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, FactionBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUFaction) * FactionCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_COPY_READ_BUFFER, FactionOutputBuffer);
	glBufferStorage(GL_COPY_READ_BUFFER, sizeof(GPUFaction) * FactionCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_COPY_READ_BUFFER, EntityOutputBuffer);
	glBufferStorage(GL_COPY_READ_BUFFER, sizeof(GPUEntity) * MaxParticleCount, NULL, GL_DYNAMIC_STORAGE_BIT);
	glEnable(GL_TEXTURE_3D);
	glGenTextures(TexturePheremone.size(), &TexturePheremone[0]);
	for (int i = 0; i < TexturePheremone.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_3D, TexturePheremone[i]);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RG16F, PheremoneSize, PheremoneSize, FactionCount, 0, GL_RG, GL_FLOAT,NULL);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindImageTexture(i, TexturePheremone[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG16F);
	}
	glGenTextures(1, &TexturePheremonePaint);
	glBindTexture(GL_TEXTURE_2D, TexturePheremonePaint);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PheremoneSize, PheremoneSize, 0, GL_RGBA, GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindImageTexture(4, TexturePheremonePaint, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


	//World initilisation
//	InitWorld();
}


WaterEngineGPU::~WaterEngineGPU()
{
}

void WaterEngineGPU::Update(float realdt)
{
	TimeScalingFactor = std::clamp(TimeScalingFactor, 1.0f / (1 << 8), (float)(1 << 4));
	//TimeScalingFactor = 1;
	DtAccumulator += realdt * TimeScalingFactor;
	int dtstepcount = std::min(10, static_cast<int>(DtAccumulator / this->DeltaTime));
	DtAccumulator -= this->DeltaTime * static_cast<float>(dtstepcount);
	DtAccumulator = std::min((float)this->DeltaTime*2, DtAccumulator);
	//dtstepcount = 1;
	//	glMemoryBarrier(GL_ALL_BARRIER_BITS);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneBuffer);
//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUScene), &GPUscene);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	for (int i = 0; i < FactionCount; ++i) {
		if (WorldFactionMirrorDirty[i]) {
			WorldFactionMirrorDirty[i] = false;
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, FactionBuffer);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUFaction), &WorldFactionMirror[i]);
		}
	}
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	for (int i = 0; i < dtstepcount; ++i)
	{
		UpdateStep();
	}
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneBuffer);
	glBindBuffer(GL_COPY_READ_BUFFER, SceneOutputBuffer);
	glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, sizeof(GPUScene));
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(GPUScene), &GPUscene);
	if (FollowEntity > 0 && FollowEntity < MaxParticleCount)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, EntityBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, EntityOutputBuffer);
		glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, FollowEntity * sizeof(GPUEntity) , FollowEntity * sizeof(GPUEntity) , sizeof(GPUEntity));
		glGetBufferSubData(GL_COPY_READ_BUFFER, FollowEntity * sizeof(GPUEntity), sizeof(GPUEntity), &WorldEntityMirror[FollowEntity]);
		if (WorldEntityMirror[FollowEntity].ToRemove == 1) 
		{
			FollowEntity = -1;
		}
	}
	else
	{
		FollowEntity = -1;
	}
	if (EntityUpdateCounter-- <= 0)
	{
		EntityUpdateCounter = 10;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, EntityBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, EntityOutputBuffer);
		glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, 0 , 0 , sizeof(GPUEntity) * GPUscene.ParticleCount);
		glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(GPUEntity) * GPUscene.ParticleCount , WorldEntityMirror.data());
//		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUEntity) * GPUscene.ParticleCount, WorldEntityMirror.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, FactionBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, FactionOutputBuffer);
		glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, 0 , 0 , sizeof(GPUFaction) * FactionCount);
		glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(GPUFaction) * FactionCount, WorldFactionMirror.data());
	}
	ParticleCount = GPUscene.ParticleCount;
}
void WaterEngineGPU::UpdateStep(bool removedead,bool chunkupdate,bool entityupdate)
{
//	Program_EntityUpdate.UseProgram();
	for (int i = 0; i < TexturePheremone.size(); ++i)
	{
		glBindImageTexture(i, TexturePheremone[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG16F);
	}
	glBindImageTexture(4, TexturePheremonePaint, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	/*	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, TexturePheremone[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, TexturePheremone[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, TexturePheremone[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, TexturePheremone[3]);
	*/
	Time += DeltaTime;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, EntityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, RenderPositionBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, RenderColourBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, RenderSizeBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, SceneBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ChunkBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, FactionBuffer);
	if(removedead)
	{
		Program_RemoveDead.UseProgram();
		static constexpr int EntitiesPerThread = 10;
		static constexpr int ThreadsPerWarp = 16;
		glDispatchCompute(static_cast<int>(ceil(float(MaxParticleCount)/(EntitiesPerThread*ThreadsPerWarp))), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ChunkBuffer);
//	glClearBufferData(GL_SHADER_STORAGE_BUFFER,GL_R32I, GL_RED_INTEGER, GL_INT, NULL);
	//glClearNamedBufferData(ChunkBuffer, GL_R32I, GL_RED_INTEGER, GL_INT, NULL);
	if(chunkupdate)
	{
		Program_ChunkReset.UseProgram();
		static constexpr int LocalSize = 64;
		glDispatchCompute(static_cast<int>(ceil(float(ChunkCount * ChunkCount) / LocalSize)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	if(chunkupdate)
	{
		Program_ChunkUpdate.UseProgram();
		static constexpr int EntitiesPerThread = 10;
		static constexpr int ThreadsPerWarp = 16;
		glDispatchCompute(static_cast<int>(ceil(float(MaxParticleCount)/(EntitiesPerThread*ThreadsPerWarp))), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	if(entityupdate)
	{
		Program_CollisionUpdate.UseProgram();
		glUniform1f(UniformCollisionRandomTimeSeed, Time + random_number(generator));
		static constexpr int LocalSize = 128;
		glDispatchCompute(static_cast<int>(ceil(float(ChunkCount*ChunkCount)/LocalSize)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	if (entityupdate && AIUpdateTick-- <= 0)
	{
		AIUpdateTick = 1;
		Program_EntityAIUpdate.UseProgram();
		glUniform1i(UniformEntityAIEntityCount, GPUscene.ParticleCount);
		glUniform1f(UniformEntityAIWorldSize, WorldSize);
		glUniform1f(UniformEntityAIRandomTimeSeed, Time + random_number(generator));
		static const constexpr int LocalSize = 512;
		glDispatchCompute(static_cast<int>(ceil(float(MaxParticleCount) / LocalSize)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
	if (entityupdate)
	{
		Program_EntityUpdate.UseProgram();
		glUniform1i(UniformEntityEntityCount, GPUscene.ParticleCount);
		glUniform1f(UniformEntityWorldSize, WorldSize);
		glUniform1f(UniformEntityRandomTimeSeed, Time + random_number(generator));
		static const constexpr int LocalSize = 512;
		glDispatchCompute(static_cast<int>(ceil(float(MaxParticleCount) / LocalSize)), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
	if(true)
	{
		Program_PheremoneUpdate.UseProgram();
		for (int i = 0; i < TexturePheremone.size(); ++i)
		{
			glUniform1i(UniformUpdatePheremone, i);
			glUniform1f(UniformPHUpdateDecayRate, PhermoneDecayRate[i]);
			glUniform1f(UniformPHUpdateDiffusionConst, PhermoneDiffusion[i]);
			static constexpr int CompSize = 16;
			glDispatchCompute(PheremoneSize / CompSize, PheremoneSize / CompSize, FactionCount);
		}
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	if (PheremonePainterDirty)
	{
		if (PheremonePainterDirtyCounter++ >= 1)
		{
			PheremonePainterDirtyCounter = 0;
			Program_PheremonePaint.UseProgram();
			PheremonePainterDirty = false;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TexturePheremonePaint);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PheremoneSize, PheremoneSize, 0, GL_RGBA, GL_FLOAT, PheremonePainter.data());
			glUniform1i(UniformPhermonePaint, 2);
			glUniform1i(UniformPaintAffiliation, PheremonePaintAffilation);
			static constexpr int CompSize = 16;
			glDispatchCompute(PheremoneSize / CompSize, PheremoneSize / CompSize, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			std::fill(PheremonePainter.begin(), PheremonePainter.end(), 0);
		}
	}

/*
*/
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER,0 );
}
int WaterEngineGPU::ResetWorld()
{
	glClearNamedBufferData(EntityBuffer, GL_R32F, GL_RED, GL_FLOAT, NULL);
	glClearNamedBufferData(RenderPositionBuffer, GL_R32F, GL_RED, GL_FLOAT, NULL);
	glClearNamedBufferData(RenderColourBuffer, GL_R32F, GL_RED, GL_FLOAT, NULL);
	glClearNamedBufferData(RenderSizeBuffer, GL_R32F, GL_RED, GL_FLOAT, NULL);
	glClearNamedBufferData(SceneBuffer, GL_R32F, GL_RED, GL_FLOAT, NULL);
	glClearNamedBufferData(ChunkBuffer, GL_R32I, GL_RED_INTEGER, GL_INT, NULL);
	GPUscene = GPUScene();
	for (int i = 0; i < TexturePheremone.size(); ++i)
	{
		glClearTexImage(TexturePheremone[i], 0, GL_RGBA, GL_FLOAT,NULL);
	}
	return InitWorld();
}
int WaterEngineGPU::InitWorld()
{
	std::cout << "Generating world\n";
	static constexpr int MaxMaskEnts = 10000;
	constexpr int Nests = FactionCount;
	constexpr int Prey = 2000;
	constexpr int Pred = 15;
	int i = 0;
	constexpr float NoiseMaskSizeResolution = 16;
	constexpr int NoiseMaskSize = 2 * WorldSize / NoiseMaskSizeResolution;
	constexpr float Frequency = NoiseMaskSize/4.0;
	std::vector<float> WorldNoiseMask(NoiseMaskSize * NoiseMaskSize);
	WorldEntityMirror = std::vector<GPUEntity>(MaxParticleCount);
	float MaxMaskVal = 0.6;
	for (int x = 0; x < NoiseMaskSize; ++x)
	{
		for (int y = 0; y < NoiseMaskSize; ++y)
		{
			WorldNoiseMask[y + (x * NoiseMaskSize)] = PerlinGenerator.accumulatedOctaveNoise2D_0_1(float(x) / Frequency, float(y) / Frequency, 4);
			float Dist = glm::length(glm::vec2(NoiseMaskSize / 2.0, NoiseMaskSize / 2.0) - glm::vec2(float(x), float(y)));
			WorldNoiseMask[y + (x * NoiseMaskSize)] *= (1.0 - std::pow(std::min(NoiseMaskSize/2.0f,Dist) / (NoiseMaskSize/2.0),8));
		}
	}
//	MaxMaskVal = *std::max_element(WorldNoiseMask.begin(), WorldNoiseMask.end());
	//std::transform(WorldNoiseMask.begin(), WorldNoiseMask.end(), WorldNoiseMask.begin(), [&MaxMaskVal](auto& c) { return c > MaxMaskVal; });
	std::ofstream maskout("mapmask.png", std::ios::binary);
	std::vector<uint8_t> outmap(NoiseMaskSize * NoiseMaskSize * 3);
	for (int x = 0; x < NoiseMaskSize; ++x)
	{
		for (int y = 0; y < NoiseMaskSize; ++y)
		{
			int i = x + (y * NoiseMaskSize);
			outmap[i * 3] = static_cast<uint8_t>((WorldNoiseMask[i] > MaxMaskVal) * 255.0);
			outmap[i * 3 + 1] = 0;
			outmap[i * 3 + 2] = static_cast<uint8_t>((WorldNoiseMask[i]) * 255.0);
		}
	}
	TinyPngOut pngout(static_cast<uint32_t>(NoiseMaskSize), static_cast<uint32_t>(NoiseMaskSize), maskout);
	pngout.write(outmap.data(), static_cast<size_t>(NoiseMaskSize * NoiseMaskSize));
	int Size = NoiseMaskSizeResolution;
	
	std::vector<float> WorldNoiseMaskMutable = WorldNoiseMask;
	int MaskVal = 0;
	for (; i < MaxMaskEnts; ++i)
	{
		auto max = max_element(WorldNoiseMaskMutable.begin(), WorldNoiseMaskMutable.end());
		int id = distance(WorldNoiseMaskMutable.begin(), max);
		if(*max > MaxMaskVal)
		{
			int x = floor(id / NoiseMaskSize);
			int y = id - (x * NoiseMaskSize);
			auto& e = WorldEntityMirror[i];
			e.PositionX = (x * NoiseMaskSizeResolution) - WorldSize;
			e.PositionY = (y * NoiseMaskSizeResolution) - WorldSize;
			e.PositionOldX = e.PositionX;
			e.PositionOldY = e.PositionY;
			e.Size = Size;
			e.MaxSize = e.Size;
			e.ColourR = 1;
			e.ColourG = 1;
			e.ColourB = 1;
			e.Mass = 1000000;
			e.Type = 4;
			GPUscene.TypeCount[int(e.Type)]++;
			int MaskRemoveSize = 0;//int(Size/NoiseMaskSizeResolution);
			for (int dx = -MaskRemoveSize; dx <= MaskRemoveSize; ++dx)
			{
				for (int dy = -MaskRemoveSize; dy <= MaskRemoveSize; ++dy)
				{
					int xa = x + dx;
					int ya = y + dy;
//					if (x + dx >= 0 && x + dx < NoiseMaskSize && y + dy >= 0 && y + dy < NoiseMaskSize)
					if (xa < 0) {
						xa += NoiseMaskSize;
					}
					if (xa >= NoiseMaskSize) {
						xa -= NoiseMaskSize;
					}
					if (ya < 0) {
						ya += NoiseMaskSize;
					}
					if (ya >= NoiseMaskSize) {
						ya -= NoiseMaskSize;
					}
					{
						WorldNoiseMaskMutable[ya + (xa * NoiseMaskSize)] = 0;
					}
				}
			} 
//			GPUscene.IndirectionId[int(e.Type)][GPUscene.TypeCount[int(e.Type)]++] = i;
		}
		else {
			break;
		}
	}
	std::cout << "World realised in "<<i<<" pseudo entities\n";
	MaskVal = i;
	int EntityCounter = MaskVal + Nests + Prey + Pred;
	auto getmask = [&](float x, float y) {
		int xmask = floor((x + WorldSize) / NoiseMaskSizeResolution);
		int ymask = floor((y + WorldSize) / NoiseMaskSizeResolution);
		xmask = std::clamp(xmask, 0, NoiseMaskSize - 1);
		ymask = std::clamp(ymask, 0, NoiseMaskSize - 1);
		return WorldNoiseMask[ymask + (xmask * NoiseMaskSize)];
	};
	//Gen the starting stuff
	int StartNest = i;
	float SpawnMaskVal = MaxMaskVal * 0.65;
	for(int af = 0;i < MaskVal + Nests;++i,++af)
	{
		auto& e = WorldEntityMirror[i];
		e.PositionX = random_number(generator) * WorldSize;
		e.PositionY = random_number(generator) * WorldSize;
		while (getmask(e.PositionX,e.PositionY) > SpawnMaskVal)
		{
			e.PositionX = random_number(generator) * WorldSize;
			e.PositionY = random_number(generator) * WorldSize;
		}
		if (i == 0)
		{
//			e.PositionX = 0;
//			e.PositionY = 0;
		}
		e.PositionOldX = e.PositionX;
		e.PositionOldY = e.PositionY;
//		e.PositionOldX += random_number(generator) * 1;
//		e.PositionOldY += random_number(generator) * 1;
		e.Mass = 10000000;
		e.MaxEnergy = 100000;
		e.Energy = 8'000;
		e.MaxHealth = 500;
		e.Health = 500;
		e.Type = 1;
		e.Age = 600;
		e.Size = 50;
		e.MaxSize = e.Size;
		e.ColourR = (random_number(generator) + 1) * 0.5;
		e.ColourG = (random_number(generator) + 1) * 0.5;
		e.ColourB = (random_number(generator) + 1) * 0.5;
		e.Affiliation = af;
		GPUscene.TypeCount[int(e.Type)]++;
//		GPUscene.IndirectionId[int(e.Type)][GPUscene.TypeCount[int(e.Type)]++] = i;
	}
	for(;i < MaskVal + Nests + Prey;++i)
	{
		auto& e = WorldEntityMirror[i];
		e.PositionX = random_number(generator) * WorldSize;
		e.PositionY = random_number(generator) * WorldSize;
		while (getmask(e.PositionX, e.PositionY) > SpawnMaskVal)
		{
			e.PositionX = random_number(generator) * WorldSize;
			e.PositionY = random_number(generator) * WorldSize;
		}
		e.PositionOldX = e.PositionX;
		e.PositionOldY = e.PositionY;
//		e.PositionOldX += random_number(generator) * 1;
//		e.PositionOldY += random_number(generator) * 1;
		e.Size = 10 * (1+random_number(generator));
		e.MaxSize = e.Size;
		e.MaxEnergy = e.Size * e.Size * 100;
		e.Energy = (e.MaxEnergy) * abs(random_number(generator));
		e.Age = 40 * abs(random_number(generator));
		e.Mass = 20;
		e.Type = 2;
		e.ColourR = 0;
		e.ColourG = 1;
		e.ColourB = 0;
		e.Affiliation = -1;
		GPUscene.TypeCount[int(e.Type)]++;
//		GPUscene.IndirectionId[int(e.Type)][GPUscene.TypeCount[int(e.Type)]++] = i;
	}
	for(;i < MaskVal + Nests + Prey + Pred;++i)
	{
		auto& e = WorldEntityMirror[i];
		e.PositionX = random_number(generator) * WorldSize;
		e.PositionY = random_number(generator) * WorldSize;
		while (getmask(e.PositionX, e.PositionY) > SpawnMaskVal)
		{
			e.PositionX = random_number(generator) * WorldSize;
			e.PositionY = random_number(generator) * WorldSize;
		}
		e.PositionOldX = e.PositionX;
		e.PositionOldY = e.PositionY;
//		e.PositionOldX += random_number(generator) * 1;
//		e.PositionOldY += random_number(generator) * 1;
		e.Mass = 40;
		e.Age = 80;
		e.Type = 3;
		e.Size = 20;
		e.MaxSize = e.Size;
		e.ColourR = 1;
		e.ColourG = 0;
		e.ColourB = 0;
		e.Affiliation = -2;
		e.MaxHealth = 100;
		e.Health = 100;
		e.MaxEnergy = 50000;
		e.Energy = 2000;
		GPUscene.TypeCount[int(e.Type)]++;
//		GPUscene.IndirectionId[int(e.Type)][GPUscene.TypeCount[int(e.Type)]++] = i;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EntityBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER,0, sizeof(GPUEntity) * EntityCounter, WorldEntityMirror.data());
	GPUscene.ParticleCount = EntityCounter;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SceneBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUScene), &GPUscene);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	WorldFactionMirror = std::vector<GPUFaction>(FactionCount);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, FactionBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUFaction) * WorldFactionMirror.size(), &GPUscene);
	for (int i = 0; i < 10; ++i)
	{
		UpdateStep(false, true, false);
	}
	return StartNest;
}