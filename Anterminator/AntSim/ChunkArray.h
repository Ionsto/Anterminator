#pragma once
#include "Chunk.h"
#include <vector>
template<int ws>
class ChunkArray
{
	static constexpr int WorldSize = ws;
public:
	static constexpr int ChunkSize = Chunk::ChunkSize;
	static constexpr int ChunkCount =static_cast<int>((WorldSize/Chunk::ChunkSize));
	std::vector<Chunk> raw_data = std::vector<Chunk>(ChunkCount*ChunkCount);
	Chunk& GetChunk(float x, float y)
	{
		int ix = (1.0 + (x / WorldSize)) * (ChunkCount / 2.0);
		int iy = (1.0 + (y / WorldSize)) * (ChunkCount / 2.0);
		ix = std::clamp(ix, 0, ChunkCount - 1);
		iy = std::clamp(iy, 0, ChunkCount - 1);
		return raw_data[(ix * ChunkCount) + iy];
	}
	std::tuple<int,int> GetChunkPos(float x, float y)
	{
		int ix = (1.0 + (x / WorldSize)) * (ChunkCount / 2.0);
		int iy = (1.0 + (y / WorldSize)) * (ChunkCount / 2.0);
		ix = std::clamp(ix, 0, ChunkCount - 1);
		iy = std::clamp(iy, 0, ChunkCount - 1);
		return { ix,iy };
	}
	Chunk& GetChunkGrid(int ix, int iy)
	{
		ix = std::clamp(ix, 0, ChunkCount - 1);
		iy = std::clamp(iy, 0, ChunkCount - 1);
		return raw_data[(ix * ChunkCount) + iy];
	}
};

