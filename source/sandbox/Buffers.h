#pragma once
#include <vulkan/vulkan.h>

void CopyBuffers(VkDevice& device, VkCommandPool& cmdPool, VkQueue queue, VkBuffer srcBuffer,
	VkBuffer dstBuffer, VkDeviceSize size);

class Buffer
{
public:
	void Init(VkDevice& device, VkPhysicalDevice& physicalDevice, 
		uint32_t sizeOfBuffer, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties);
	void InitAsVertexBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VkQueue queue,
		uint32_t sizeOfBuffer, void* data);

	void InitAsIndexBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VkQueue queue,
		uint32_t sizeOfBuffer, void* data);
		
	void Release()
	{
		vkDestroyBuffer(*m_device, m_buffer, nullptr);
		vkFreeMemory(*m_device, m_buffMemory, nullptr);
	}
	VkBuffer& GetBuffer() { return m_buffer; }
private:
	VkDevice* m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_buffMemory;

};
