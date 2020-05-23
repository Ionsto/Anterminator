#pragma once
#include <glm/glm.hpp>
class Camera
{
public:
	glm::fvec2 Position = glm::fvec2(0,0);
	float Height = 0;
	static constexpr float HeightMin = 0;
	static constexpr float HeightMax = 0;
};

