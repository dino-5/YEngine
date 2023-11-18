#include <stdexcept>
#include "CommandPool.h"
#include "Queue.h"
#include "Device.h"
#include "Surface.h"

void CommandPool::init(Device& device, Surface surface)
{
	m_device = &device.getLogicalDevice().getDevice();

	QueueFamilyIndices indices = FindQueueFamilies(device.getPhysicalDevice().getDevice(), surface.getSurface());
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

	if (vkCreateCommandPool(*m_device, &poolCreateInfo, nullptr, &m_cmdPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}
