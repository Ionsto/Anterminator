#pragma once
#include <glm/glm.hpp>
#include "Chunk.h"
class Camera
{
public:
	glm::fvec3 Position = glm::fvec3(0,0,0);
	float Height = 0;
	static constexpr float HeightMin = 0;
	static constexpr float HeightMax = 0;
	float yaw = 0;
	float tilt = 0;
	float CameraXSize = Chunk::Size * 2;
	float CameraYSize = Chunk::Size * 2;
};

