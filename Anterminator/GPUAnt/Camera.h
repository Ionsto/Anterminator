#pragma once
#include <glm/glm.hpp>
class Camera
{
public:
	glm::fvec3 Position = glm::fvec3(0,0,0);
	float Height = 0;
	static constexpr float HeightMin = 0;
	static constexpr float HeightMax = 0;
	float yaw = 0;
	float tilt = 0;
	float CameraXSize = 1920/2;
	float CameraYSize = 1080/2;
};

