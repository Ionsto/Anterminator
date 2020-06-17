#pragma once
#include "Chunk.h"
#include <iostream>
#include <array>
#include <vector>
template
<int chunkcount>
class ChunkMap
{
public:
	static constexpr int ChunkCount = chunkcount;
	static constexpr float WorldSize = ChunkCount * Chunk::Size;
	//std::array<Chunk, ChunkCount * ChunkCount> raw_data;
	std::vector<Chunk> raw_data = std::vector<Chunk>(ChunkCount * ChunkCount);
	unsigned modulo(int value, unsigned m) {
		int mod = value % (int)m;
		if (value < 0 && mod != 0) {
			mod += m;
		}
		return mod;
	}
//#pragma optimize("", off)
	Chunk& GetChunk(int x_a, int y_a)
	{
		int x = modulo(x_a, ChunkCount);
		int y = modulo(y_a, ChunkCount);
		//raw_data[y + (x * ChunkCount)].X = x_a * Chunk::Size;
		//raw_data[y + (x * ChunkCount)].Y = y_a * Chunk::Size;
		return raw_data[y + (x * ChunkCount)];
	}
//#pragma optimize("", on)
	Chunk& GetChunkPos(float x, float y)
	{
		//if (x >= 0 && y >= 0 && x < WorldSize * Chunk::Size && y < WorldSize * Chunk::Size)
		//{
		int ix = floor(x / Chunk::Size);
		int iy = floor(y / Chunk::Size);
		return GetChunk(ix, iy);
		//return raw_data[iy + (ix * WorldSize)];
		//}
		//std::cout << "Erronous chunk access\n";
		//throw;
	}
	std::tuple<int,int> GetChunkId(float x, float y)
	{
		int ix = floor(x / Chunk::Size);
		int iy = floor(y / Chunk::Size);
		return { ix,iy };
	}
	bool InBounds(int cx, int cy)
	{
		return cx >= 0 && cy >= 0 && cx < ChunkCount && cy < ChunkCount;
	}
	bool InBoundsPos(float cx, float cy)
	{
		return cx >= 0 && cy >= 0 && cx < WorldSize && cy < WorldSize;
	}
	ChunkMap()
	{
		for (int x = 0; x < ChunkCount; ++x)
		{
			for (int y = 0; y < ChunkCount; ++y)
			{
				GetChunk(x, y) = Chunk(x * Chunk::Size, y * Chunk::Size);
			}
		}

	}
	void Update()
	{
		for (auto& c : this->raw_data) {
			c.Update();
		}
	}
};

