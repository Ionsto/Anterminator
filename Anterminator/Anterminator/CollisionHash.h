#pragma once
#include <array>
struct CollisionNode {
	static constexpr int MaxEntities = 500;
	int EntityCount = 0;
	std::array<int, MaxEntities> EntityIDList;
	void AddEntity(int i)
	{
		if (EntityCount < MaxEntities) {
			EntityIDList[EntityCount++] = i;
		}
	}
};
class CollisionHash
{
	static constexpr float HashSize = 10;
	static constexpr int HashCount = 100;
	static constexpr int NodeSize = HashSize/HashCount;
	float X, Y;
	std::array<CollisionNode, HashCount* HashCount> Nodes;
public:
	void Clear() {
		for (auto& n : Nodes)
		{
			n.EntityCount = 0;
		}
	}
	void AddEntity(int id, float x, float y) {
		int ix = std::floor((x - X) / NodeSize);
		int iy = std::floor((y - Y) / NodeSize);
		Nodes[ix + (iy * HashCount)].AddEntity(id);
	}
};

