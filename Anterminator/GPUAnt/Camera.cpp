#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
Camera::Camera() {
	Projection = glm::perspective(glm::radians(FOV), CameraXSize / CameraYSize, 0.1f, 100.0f);
	MVPMat = glm::identity<glm::mat4>();
}
void Camera::Update() {
	Projection = glm::perspective(glm::radians(FOV), CameraXSize / CameraYSize, ClipNear, ClipFar);

	View = glm::translate(glm::mat4(1.f), -Position);
//	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Orientation.y, Up);
//	glm::mat4 View = glm::rotate(ViewRotateX, Orientation.x, Up);

//	glm::mat4 Model = glm::mat4(1.0f);
	MVPMat = Projection * View;
}
