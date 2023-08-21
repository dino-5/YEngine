#pragma once
#include <vulkan/vulkan.h>

class PhysicalDevice
{
public:
	void Init(VkSurfaceKHR surface);
	VkPhysicalDevice& GetDevice() { return m_device; }
private:
	VkPhysicalDevice m_device = nullptr;
};


class LogicalDevice
{
public:
	void Init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);
	void Destroy()
	{

		vkDestroyDevice(m_device, nullptr);
	}

private:
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkDevice m_device;
};

