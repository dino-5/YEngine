#pragma once
#include <vulkan/vulkan.h>
#include "gfx_common.h"

void copyBuffers(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& physicalDevice);

class Buffer
{
public:
	DesctructorVulkanObject(Buffer)

	void init(uint32_t sizeOfBuffer, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties);
	void initAsVertexBuffer(uint32_t sizeOfBuffer, void* data);

	void initAsStagingBuffer(uint32_t sizeOfBuffer, void* data );

	void initAsIndexBuffer(uint32_t sizeOfBuffer, void* data);

	void mapMemory(uint32_t offset) {
		vkMapMemory(*m_device, m_buffMemory, offset, m_bufferSize, 0, &m_mappedMemory);
	}
	void unmapMemory()
	{
		vkUnmapMemory(*m_device, m_buffMemory);
	 }

	VkDescriptorBufferInfo getDescriptorBufferInfo() {
		VkDescriptorBufferInfo desc;
		desc.buffer = m_buffer;
		desc.offset = 0;
		desc.range = m_bufferSize;
		return desc;
	}
		
	void release()
	{
		VulkanObjectReleased();
		vkDestroyBuffer(*m_device, m_buffer, nullptr);
		vkFreeMemory(*m_device, m_buffMemory, nullptr);
	}
	void copyMemory(void* data);
	VkBuffer& getBuffer() { return m_buffer; }
private:
	void* m_mappedMemory = nullptr;
	uint32_t m_bufferSize;
	VkDevice* m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_buffMemory;

};
