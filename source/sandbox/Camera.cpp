#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera::Init(float aspectRatio)
{
	m_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
}
