#pragma once
#include <GLFW/glfw3.h>
#include <utility>
#include <array>
class KeyManager
{
public:
	static constexpr int KeyCount = 258;
private:
	std::array<bool,KeyCount> State = { false };
	std::array<bool,KeyCount> Toggle = { false };
public:
	void UpdateState(GLFWwindow* window);
	std::pair<bool, bool> GetState(int Key);
};

