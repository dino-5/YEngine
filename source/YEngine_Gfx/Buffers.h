#pragma once
#include <vulkan/vulkan.h>
#include <functional>

#include "gfx_common.h"
#include "GraphicsPipeline.h"
#include "YEngine_System/system/Logger.h"

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void copyBuffers(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& physicalDevice);

class Buffer
{
public:
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
		if (m_isReleased)
			Logger::PrintError("trying to release already release buffer");
		vkDestroyBuffer(*m_device, m_buffer, nullptr);
		vkFreeMemory(*m_device, m_buffMemory, nullptr);
		m_isReleased = true;
	}
	~Buffer()
	{
		if (!m_isReleased)
			throw std::runtime_error("Buffer is not destroyed");
	}
	void copyMemory(void* data);
	VkBuffer& getBuffer() { return m_buffer; }
private:
	bool m_isReleased = true;
	void* m_mappedMemory = nullptr;
	uint32_t m_bufferSize;
	VkDevice* m_device;
	VkBuffer m_buffer;
	VkDeviceMemory m_buffMemory;

};

template<typename Internal>
struct UniformBuffer
{
	UniformBuffer(uint32_t binding, ShaderType type) : m_binding(binding), m_type(type) {}
	VkDescriptorSetLayoutBinding GetDescriptorSetBinding()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = m_binding;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = castToNativeEnum(m_type);
		uboLayoutBinding.pImmutableSamplers = nullptr;
		return uboLayoutBinding;
	}

	void init()
	{
		for (auto& buffer : m_buffers)
		{
			buffer.init(sizeof(Internal::Data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			buffer.mapMemory(0);
		}

		m_data.init();
	}
	void update(uint32_t index)
	{
		m_data.update();
		m_buffers[index].copyMemory(&m_data.data);
	}

	void release()
	{
		for (auto& buffer : m_buffers)
			buffer.release();
	}
	void setView(glm::mat4 mat) { m_data.view = mat; }
	void setProj(glm::mat4 mat) { m_data.proj = mat; }

	Internal m_data;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_buffers;
	uint32_t m_binding;
	ShaderType m_type;
};
