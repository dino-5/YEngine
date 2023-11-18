#pragma once
#include <stdexcept>
#include <memory>
#include "YEngine_System/common.h"
#include "Buffers.h"
#include "GraphicsModule.h"

using namespace graphics;

void Buffer::copyMemory(void* data) {
	memcpy(m_mappedMemory, data, m_bufferSize);
}

void copyBuffers(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkCommandPool& cmdPool = GraphicsModule::GetInstance()->getCommandPool().getPool();
	VkQueue queue = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getGraphicsQueue();
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = cmdPool;
	allocInfo.commandBufferCount = 1;

	//todo: make it with CommandBuffer class
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, cmdPool, 1, &commandBuffer);

}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& physicalDevice)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties)==properties)
			return i;
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Buffer::init( uint32_t sizeOfBuffer, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties)
{
	VulkanObjectInitialized();
	m_device = &GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkPhysicalDevice physicalDevice = GraphicsModule::GetInstance()->getDevice().getPhysicalDevice().getDevice();
	m_bufferSize = sizeOfBuffer;
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeOfBuffer;
	bufferInfo.usage = bufferUsage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(*m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer");

	VkMemoryRequirements memRequirements{};
	vkGetBufferMemoryRequirements(*m_device, m_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryProperties, physicalDevice);

	if (vkAllocateMemory(*m_device, &allocInfo, nullptr, &m_buffMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to alocate memory for buffer");

	vkBindBufferMemory(*m_device, m_buffer, m_buffMemory, 0);

}

void Buffer::initAsStagingBuffer(uint32_t sizeOfBuffer, void* data)
{
	init(sizeOfBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	mapMemory(0);
	copyMemory(data);
	unmapMemory();
}

void Buffer::initAsVertexBuffer(uint32_t sizeOfBuffer, void* data)
{
	Buffer stagingBuffer;
	stagingBuffer.initAsStagingBuffer( sizeOfBuffer, data);

	init(sizeOfBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffers(stagingBuffer.m_buffer, m_buffer, static_cast<VkDeviceSize>(sizeOfBuffer));
	stagingBuffer.release();
}

void Buffer::initAsIndexBuffer(uint32_t sizeOfBuffer, void* data)
{
	Buffer stagingBuffer;
	stagingBuffer.initAsStagingBuffer(sizeOfBuffer, data);

	init(sizeOfBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffers(stagingBuffer.m_buffer, m_buffer, static_cast<VkDeviceSize>(sizeOfBuffer));
	stagingBuffer.release();
}
