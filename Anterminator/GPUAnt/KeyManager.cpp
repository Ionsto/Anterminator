#include "KeyManager.h"
#include <iostream>

void KeyManager::UpdateState(GLFWwindow* window)
{
	for (int i = 0; i < KeyCount; ++i)
	{
		Toggle[i] = false;
		bool Newstate = glfwGetKey(window,i);
		if (Newstate != State[i])
		{
			Toggle[i] = true;
			State[i] = Newstate;
		}

	}
}
std::pair<bool, bool> KeyManager::GetState(int Key)
{
	if (Key < 0 || Key >= KeyCount)
	{
		std::cout << "Invalid key\n";
		return { false,false };
	}
	return { State[Key],Toggle[Key] };
}
