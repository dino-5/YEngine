#pragma once
#include <vulkan/vulkan.h>
#include "common.h"

void CopyBuffers(VkDevice& device, VkCommandPool& cmdPool, VkQueue queue, VkBuffer srcBuffer,
	VkBuffer dstBuffer, VkDeviceSize size);

class Buffer
{
public:
	DesctructorVulkanObject(Buffer)

	void Init(VkDevice& device, VkPhysicalDevice& physicalDevice, 
		uint32_t sizeOfBuffer, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties);
	void InitAsVertexBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VkQueue queue,
		uint32_t sizeOfBuffer, void* data);

	void InitAsIndexBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VkQueue queue,
		uint32_t sizeOfBuffer, void* data);

	void MapMemory(uint32_t offset) {
		vkMapMemory(*m_device, m_buffMemory, offset, m_bufferSize, 0, &m_mappedMemory);
	}
	void UnmapMemory()
	{
		vkUnmapMemory(*m_device, m_buffMemory);
	 }
		
	void Release()
	{
		VulkanObjectReleased();
		vkDestroyBuffer(*m_device, m_buffer, nullptr);
		vkFreeMemory(*m_device, m_buffMemory, nullptr);
	}
	void CopyMemory(void* data) {
		memcpy(m_mappedMemory, data, m_bufferSize);
	}
	VkBuffer& GetBuffer() { return m_buffer; }
private:
	void* m_mappedMemory = nullptr;
	uint32_t m_bufferSize;
	VkDevice* m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_buffMemory;

};
