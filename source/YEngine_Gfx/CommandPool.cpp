#include <stdexcept>
#include "CommandPool.h"
#include "Queue.h"
#include "Device.h"
#include "Surface.h"

void CommandPool::init(Device& device, Surface surface, uint32_t numberOfCmdBuffers)
{
	m_device = &device.getLogicalDevice().getDevice();

	QueueFamilyIndices indices = FindQueueFamilies(device.getPhysicalDevice().getDevice(), surface.getSurface());
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

	if (vkCreateCommandPool(*m_device, &poolCreateInfo, nullptr, &m_cmdPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");

	// create command buffers
	m_cmdBuffers.resize(numberOfCmdBuffers);
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = m_cmdPool;
	cmdBufferAllocInfo.commandBufferCount = numberOfCmdBuffers;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(*m_device, &cmdBufferAllocInfo, m_cmdBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
}


CommandBuffer CommandPool::createOneTimeCmdBuffer()
{
	VkCommandBuffer cmdBuffer;
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = m_cmdPool;
	cmdBufferAllocInfo.commandBufferCount = 1;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(*m_device, &cmdBufferAllocInfo, &cmdBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
	return CommandBuffer(cmdBuffer);
}
