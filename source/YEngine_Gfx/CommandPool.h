#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "CommandBuffer.h"
class Device;
class Surface;

class CommandPool
{
public:
	void init(Device& device, Surface surface, uint32_t numberOfCmdBuffers);
	void release()
	{
		vkDestroyCommandPool(*m_device, m_cmdPool, nullptr);
	}
	VkCommandPool& getPool() { return m_cmdPool; }
	CommandBuffer createOneTimeCmdBuffer();
	CommandBuffer operator[](uint32_t index) { return CommandBuffer(m_cmdBuffers[index]); }
private:
	VkCommandPool m_cmdPool;
	std::vector<VkCommandBuffer> m_cmdBuffers;
	//std::vector<std::shared_ptr<CommandBuffer>> m_cmdBuffers;
	VkDevice* m_device;
};

