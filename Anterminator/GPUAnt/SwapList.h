#pragma once
#pragma once
#include <iostream>
template<class t, int max>
class SwapList {
public:
	static constexpr int MaxElementsPerElement = max;
	std::array<t, MaxElementsPerElement> ElementList;
	int ElementCount = 0;
	t& GetElement(int i)
	{
		return ElementList[i];
	}
	std::array<t, MaxElementsPerElement> RawData()
	{
		return ElementList;
	}
	bool AddElement(t add)
	{
		if (ElementCount != MaxElementsPerElement)
		{
			ElementList[ElementCount++] = add;
			return true;
		}
		//std::cout << "Failed to add Element\n";
		return false;
	}
	void RemoveElement(int position)
	{
		if (position == ElementCount - 1)
		{
			ElementCount--;
			return;
		}
		if (position < ElementCount)
		{
			ElementList[position] = ElementList[ElementCount-- - 1];
		}
		else {
			throw;
			//wtf
		}
	}
};
