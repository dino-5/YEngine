#pragma once
#include<vulkan/vulkan.h>
class Device;
class Surface;

class CommandPool
{
public:
	void init(Device& device, Surface surface);
	void release()
	{
		vkDestroyCommandPool(*m_device, m_cmdPool, nullptr);
	}
	VkCommandPool& getPool() { return m_cmdPool; }
private:
	VkCommandPool m_cmdPool;
	VkDevice* m_device;
};

