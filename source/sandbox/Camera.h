#pragma once
#include <glm/glm.hpp>
#include "glfw/glfw3.h"

#include "Matrix.h"

class Camera
{
public:
	void init(float aspectRatio);
	void parseMouseInput(GLFWwindow* window, double xpos, double ypos);
	void parseKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	void switchCamera() { m_isActive = !m_isActive; }
	glm::mat4 getView() { return m_view; }
private:
	glm::mat4 m_view;
	glm::mat4 m_proj;
	glm::vec3 m_pos{0.f, 0.f, 0.f};
	glm::vec3 m_direction{0.f, 1.f, 0.f};
	bool m_isActive = false;;
};

