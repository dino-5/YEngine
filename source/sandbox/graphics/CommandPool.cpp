#include <stdexcept>
#include "CommandPool.h"
#include "Queue.h"

void CommandPool::Init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface, VkDevice& device)
{
	m_device = &device;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
	VkCommandPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolCreateInfo, nullptr, &m_cmdPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");
}
