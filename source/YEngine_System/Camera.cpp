#include "Camera.h"

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <format>


constexpr const float velocity = 1;

void Camera::init(float aspectRatio)
{
	updateViewMatrix();
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
		m_pos += velocity*m_direction;
	}
	if (key == GLFW_KEY_A && action != GLFW_RELEASE)
	{

		m_pos -= velocity * glm::cross(m_direction, glm::vec3(0.f, 0.f, 1.f));
	}

	if (key == GLFW_KEY_S && action != GLFW_RELEASE)
	{
		m_pos -= velocity * m_direction;
	}

	if (key == GLFW_KEY_D && action != GLFW_RELEASE) 
	{
		m_pos += velocity * glm::cross(m_direction, glm::vec3(0.f, 0.f, 1.f));
	}
	m_view = glm::lookAt(m_pos, m_pos + m_direction, glm::vec3(0.0f, 0.0f, 1.0f));
}


void Camera::updateViewMatrix()
{
	float a = std::cos(m_x);
	float b = std::sin(m_y);
	m_direction.x = std::cos(m_x) * -std::cos(m_y);
	m_direction.y = std::sin(m_x) * std::cos(m_y);
	m_direction.z =  -std::sin(m_y);
	m_direction = glm::normalize(m_direction);
	glm::vec3 top = glm::cross(glm::cross(m_direction, glm::vec3(0.f, 0.f, 1.f)), m_direction);
	m_view = glm::lookAt(m_pos, m_pos + m_direction, top);
}

void Camera::parseMouseInput(GLFWwindow* window, double xpos, double ypos)
{
	constexpr const float sensitivity = 0.5f;
	static double s_xpos = xpos;
	static double s_ypos = ypos;
	static bool lastActive = false;

	if (!m_isActive)
	{
		if (lastActive)
			lastActive = m_isActive;
		return;
	}
	if (!lastActive)
	{
		s_xpos = xpos;
		s_ypos = ypos;
		lastActive = true;
		return;
	}
	m_x += math::Radians(sensitivity * (xpos - s_xpos));
	m_y += math::Radians(sensitivity * (ypos - s_ypos));
	s_xpos = xpos;
	s_ypos = ypos;
	updateViewMatrix();
}
