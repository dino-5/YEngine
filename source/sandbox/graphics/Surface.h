#pragma once
#include <vulkan/vulkan.h>

class Surface
{
public:
	void Init();
	void Release();
	VkSurfaceKHR& GetSurface() { return m_surface; }
private:
	VkSurfaceKHR m_surface;
};

