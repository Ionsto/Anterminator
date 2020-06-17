#pragma once
//#include "World.h"
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <algorithm>
struct PheremoneUnit {
	glm::vec2 Direction = glm::vec2(0,0);
//	float DirectionX = 0;
//	float DirectionY = 0;
	float Strength = 0;
};
struct PheremoneChunk {
	float X = 0;
	float Y = 0;
	float MaxValue = 100;
	bool Active = false;
	bool ZeroValue = true;
	static constexpr float ChunkSize = 128;
	static constexpr int GridCount = 32;
	static constexpr float Resolution = ChunkSize / GridCount;
	std::array<PheremoneUnit,GridCount * GridCount> raw_data;
	unsigned modulo(int value, unsigned m) {
		int mod = value % (int)m;
		if (value < 0 && mod != 0) {
			mod += m;
		}
		return mod;
	}
	void Update(float DecayRate, float dt);
	PheremoneUnit & GetPheremone(float x,float y)
	{
		int ix = floor(((x - X) / ChunkSize) * GridCount);
		int iy = floor(((y - Y) / ChunkSize) * GridCount);
		ix = std::clamp(ix, 0, GridCount - 1);
		iy = std::clamp(iy, 0, GridCount - 1);
		return GetPheremoneGrid(ix, iy);
	}
	PheremoneUnit& GetPheremoneGrid(int x, int y)
	{
		if (x >= 0 && x < GridCount && y >= 0 && y < GridCount)
		{
			return raw_data[y + (x * GridCount)];
		}
		return raw_data[0];
	}
};
class PheremoneGrid
{
	static constexpr float WorldSize = 4096;// World::WorldSize;
public:
	float DiffuseTime = 0;
	float DiffuseTimeMax = 0.5;
	float DecayRate = 1;
	float DiffusionConstant = 10;
	float MaxDiffusionConstant = 100;
	bool Alive = true;
	float UpdateCounterMax = 1;
	float UpdateCounter = ((rand()%1000)/1000.0) * UpdateCounterMax;
	static constexpr int ChunkCount = static_cast<int>(WorldSize/PheremoneChunk::ChunkSize);
	std::vector<PheremoneChunk> Chunks = std::vector<PheremoneChunk>(ChunkCount* ChunkCount);
	PheremoneGrid() {
		for (int x = 0; x < ChunkCount; ++x)
		{
			for (int y = 0; y < ChunkCount; ++y)
			{
				GetChunkGrid(x, y).X = x * PheremoneChunk::ChunkSize;
				GetChunkGrid(x, y).Y = y * PheremoneChunk::ChunkSize;
			}
		}
	}
	void Update(float dt);
	void Difffuse(PheremoneChunk & c, float dt)
	{
		float MeshDiffusionNumber = DiffusionConstant * (DiffuseTime) / PheremoneChunk::Resolution;
		MeshDiffusionNumber = std::min(MeshDiffusionNumber, MaxDiffusionConstant);
		float Crecip = 1.0 / (1.0f + (MeshDiffusionNumber * 4.0f));
		for (int x = 1; x < c.GridCount-1; ++x)
		{
			for (int y = 1; y < c.GridCount-1; ++y)
			{
				c.GetPheremoneGrid(x,y).Direction =
					(c.GetPheremoneGrid(x, y).Direction +
						(MeshDiffusionNumber * (
							  c.GetPheremoneGrid(x - 1, y).Direction
							+ c.GetPheremoneGrid(x + 1, y).Direction
							+ c.GetPheremoneGrid(x,y - 1).Direction
							+ c.GetPheremoneGrid(x,y + 1).Direction))) * Crecip;
				c.GetPheremoneGrid(x,y).Strength =
					(c.GetPheremoneGrid(x, y).Strength +
						(MeshDiffusionNumber * (
							  c.GetPheremoneGrid(x - 1, y).Strength
							+ c.GetPheremoneGrid(x + 1, y).Strength
							+ c.GetPheremoneGrid(x,y - 1).Strength
							+ c.GetPheremoneGrid(x,y + 1).Strength))) * Crecip;
			}
		}
		int xi = c.X / PheremoneChunk::ChunkSize;
		int yi = c.Y / PheremoneChunk::ChunkSize;
		auto& cl = GetChunkGrid(xi-1, yi);
		auto& cr = GetChunkGrid(xi+1, yi);
		auto& cu = GetChunkGrid(xi, yi + 1);
		auto& cd = GetChunkGrid(xi, yi - 1);
		auto diffusechunk = [=](PheremoneChunk& c, PheremoneChunk& cl, PheremoneChunk& cr, PheremoneChunk& cu, PheremoneChunk& cd,
			int x,
			int y
			) {
				c.GetPheremoneGrid(x,y).Direction =
					(c.GetPheremoneGrid(x, y).Direction +
						(MeshDiffusionNumber * (
							  cl.GetPheremoneGrid(modulo(x - 1,PheremoneChunk::GridCount), y).Direction
							+ cr.GetPheremoneGrid(modulo(x + 1,PheremoneChunk::GridCount), y).Direction
							+ cd.GetPheremoneGrid(x,modulo(y - 1,PheremoneChunk::GridCount)).Direction
							+ cu.GetPheremoneGrid(x,modulo(y + 1,PheremoneChunk::GridCount)).Direction))) * Crecip;
				c.GetPheremoneGrid(x,y).Strength =
					(c.GetPheremoneGrid(x, y).Strength +
						(MeshDiffusionNumber * (
							  cl.GetPheremoneGrid(modulo(x - 1,PheremoneChunk::GridCount), y).Strength
							+ cr.GetPheremoneGrid(modulo(x + 1,PheremoneChunk::GridCount), y).Strength
							+ cd.GetPheremoneGrid(x,modulo(y - 1,PheremoneChunk::GridCount)).Strength
							+ cu.GetPheremoneGrid(x,modulo(y + 1,PheremoneChunk::GridCount)).Strength))) * Crecip;
			return;
		};
		for (int v = 1; v < c.GridCount - 1; ++v)
		{
			diffusechunk(c, cl, c, c, c, 0, v);
			diffusechunk(c, c, cr, c, c, PheremoneChunk::GridCount-1, v);
			diffusechunk(c, c, c, c, cd,v, 0);
			diffusechunk(c, c, c, cu, c,v, PheremoneChunk::GridCount-1);
		}
		diffusechunk(c, cl, c, c, cd, 0, 0);
		diffusechunk(c, cl, c, cu, c, 0, PheremoneChunk::GridCount - 1);
		diffusechunk(c, c, cr, cu, c,PheremoneChunk::GridCount - 1,PheremoneChunk::GridCount - 1 );
		diffusechunk(c, c, cr, c, cd, PheremoneChunk::GridCount - 1,0);
	}
	void UpdateChunks(float dt);
	void MarkActive();
	unsigned modulo(int value, unsigned m) {
		int mod = value % (int)m;
		if (value < 0 && mod != 0) {
			mod += m;
		}
		return mod;
	}
	PheremoneChunk& GetChunkGrid(int x, int y)
	{
	//	if (x >= 0 && x < ChunkCount && y >= 0 && y < ChunkCount)
		{
			return Chunks[modulo(y,ChunkCount) + (modulo(x,ChunkCount) * ChunkCount)];
		}
//		return Chunks[0];
	}
	PheremoneChunk& GetChunk(float x, float y)
	{
		int ix = floor((x / WorldSize) * ChunkCount);
		int iy = floor((y / WorldSize) * ChunkCount);
		//ix = std::clamp(ix, 0, ChunkCount - 1);
		//iy = std::clamp(iy, 0, ChunkCount - 1);
		return GetChunkGrid(ix, iy);
	}
	PheremoneUnit& GetPheremone(float x, float y) 
	{
		auto& c = GetChunk(x, y);
		return c.GetPheremone(x, y);
	}
};

