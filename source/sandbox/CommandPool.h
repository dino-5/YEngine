#pragma once
#include<vulkan/vulkan.h>

class CommandPool
{
public:
	void Init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface, VkDevice& device);
	void Release()
	{
		vkDestroyCommandPool(*m_device, m_cmdPool, nullptr);
	}
	VkCommandPool& GetPool() { return m_cmdPool; }
private:
	VkCommandPool m_cmdPool;
	VkDevice* m_device;
};

