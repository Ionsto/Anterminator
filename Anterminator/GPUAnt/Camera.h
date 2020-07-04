#pragma once
#include <glm/glm.hpp>
class Camera
{
public:
	glm::mat4 Projection;
	glm::mat4 View;
	glm::fvec3 Position = glm::fvec3(0,0,0);
	glm::mat4 MVPMat;
	float Height = 0;
	static constexpr float HeightMin = 0;
	static constexpr float HeightMax = 0;
	float FOV = 45.0f;
	float ClipNear = 1;
	float ClipFar = 1010.0f;
	float yaw = 0;
	float tilt = 0;
	float CameraXSize = 1920/2;
	float CameraYSize = 1080/2;
	Camera();
	void Update();
};

