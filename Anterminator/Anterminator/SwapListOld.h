#pragma once
#include <array>
#include <iostream>
template<class t, int max>
class SwapList {
public:
	static constexpr int MaxSlots = max;
	std::array<t, MaxSlots> ElementList;
	std::array<int, MaxSlots> Ids;
	std::array<int, MaxSlots> ReverseIds;
	//Stable ids
	int ElementCount = 0;
	SwapList() {
		Ids.fill(-1);
	}
	t& GetParticle(int i)
	{
		return ElementList[Ids[i]];
	}
	std::array<t, MaxSlots>& RawData()
	{
		return ElementList;
	}
	int FindFreeId() {
		for (int i = 0; i < MaxSlots; ++i)
		{
			if (Ids[i] == -1)
			{
				return i;
			}
		}
		return -1;
	}
	int AddParticle(t && add)
	{
		if (ElementCount != MaxSlots)
		{
			//id is an id slot
			int id = FindFreeId();
			if (id != -1)
			{
				//Ids points to particle location
				Ids[id] = ElementCount;
				ReverseIds[ElementCount] = id;
				std::swap(ElementList[ElementCount++], add);
			}
			return id;
		}
		//std::cout << "Failed to add particle\n";
		return -1;
	}
	void RemoveParticle(int position)
	{
		if (position == ElementCount - 1)
		{

			int id = ReverseIds[position];
			Ids[id] = -1;
			ReverseIds[id] = -1;
			ElementCount--;
			return;
		}
		if (position < ElementCount)
		{
			int idend = ReverseIds[ElementCount - 1];
			int iddelete = ReverseIds[position];
			Ids[idend] = position;
			ReverseIds[position] = idend;
			Ids[iddelete] = -1;
			ReverseIds[ElementCount-1] = -1;
			std::swap(ElementList[position],ElementList[ElementCount-- - 1]);
		}
		else {
			throw;
			//wtf
		}
	}
};