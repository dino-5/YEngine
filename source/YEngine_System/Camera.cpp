#include "Camera.h"

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr const float velocity = 1;

void Camera::init(float aspectRatio)
{
	m_view = glm::lookAt(m_pos, m_pos + m_direction, glm::vec3(0.0f, 0.0f, 1.0f));
	m_proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
}

void Camera::parseKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		switchCamera();
	if (!m_isActive)
		return;
	if (key == GLFW_KEY_W && action != GLFW_RELEASE)
	{
		m_pos.y += velocity;
	}
	if (key == GLFW_KEY_A && action != GLFW_RELEASE)
	{

		m_pos.x -= velocity;
	}

	if (key == GLFW_KEY_S && action != GLFW_RELEASE)
	{
		m_pos.y -= velocity;
	}

	if (key == GLFW_KEY_D && action != GLFW_RELEASE) 
	{
		m_pos.x += velocity;
	}
	m_view = glm::lookAt(m_pos, m_pos + m_direction, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Camera::parseMouseInput(GLFWwindow* window, double xpos, double ypos)
{
	if (!m_isActive)
		return;
}
