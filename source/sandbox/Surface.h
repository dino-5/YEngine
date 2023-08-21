#pragma once
#include <vulkan/vulkan.h>

struct GLFWwindow;

class Surface
{
public:
	void Init(GLFWwindow* window);
	void Destroy();
	VkSurfaceKHR GetSurface() { return m_surface; }
private:
	VkSurfaceKHR m_surface;
};

