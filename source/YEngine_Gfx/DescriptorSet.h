#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "YEngine_System/common.h"

uint32_t GetNumberOfDescriptors(VkDescriptorPoolSize* pools, uint32_t poolsCount);
class Model;
class Buffer;
class TextureImage;

struct DescriptorSetLayoutCreateInfo
{
	VkDescriptorSetLayoutBinding* bindings;
	uint32_t count;
	DescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutBinding* t_bindings, uint32_t t_count) :
		bindings(t_bindings),
		count(t_count)
	{}
	DescriptorSetLayoutCreateInfo() = default;
};

class DescriptorSetLayout
{
public:
	void init(DescriptorSetLayoutCreateInfo createInfo);

	void release();
	VkDescriptorSetLayout& getDescriptorSetLayout() { return m_descriptorSetLayout; }
	operator VkDescriptorSetLayout() { return m_descriptorSetLayout; }
		
private:
	VkDescriptorSetLayout m_descriptorSetLayout;
};

class DescriptorPool
{
public:
	void init(VkDevice& device, VkDescriptorPoolSize* pools, uint32_t poolsCount);
	void release()
	{
		vkDestroyDescriptorPool(*m_device, m_descriptorPool, nullptr);
	}
	VkDescriptorPool& getDescriptorPool() { return m_descriptorPool; }
private:
	VkDevice* m_device;
	VkDescriptorPool m_descriptorPool;
};

class DescriptorSet
{
public:
	void init(uint32_t descriptorSetCount, DescriptorSetLayout layout);
	void updateDescriptors(VkWriteDescriptorSet* writes, uint32_t count);

	// buffers
	VkWriteDescriptorSet getWriteDescriptor(uint32_t index, uint32_t binding, Buffer& buffer);
	VkWriteDescriptorSet getWriteDescriptor(uint32_t index, uint32_t binding, VkDescriptorBufferInfo* bufferInfo,
		VkDescriptorType type);
	// textures
	VkWriteDescriptorSet getWriteDescriptor(uint32_t index, uint32_t binding, TextureImage& texture);
	VkWriteDescriptorSet getWriteDescriptor(uint32_t index, uint32_t binding, VkDescriptorImageInfo* imageInfo,
		VkDescriptorType type);
	//
	VkDescriptorSet& getDescriptorSet(uint32_t index) { return m_descriptorSet[index]; }
private:
	VkDevice* m_device;
	std::vector<VkDescriptorSet> m_descriptorSet;
};
