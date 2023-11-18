#pragma once
#include <vulkan/vulkan.h>

class Surface
{
public:
	void init();
	void release();
	VkSurfaceKHR& getSurface() { return m_surface; }
private:
	VkSurfaceKHR m_surface;
};

