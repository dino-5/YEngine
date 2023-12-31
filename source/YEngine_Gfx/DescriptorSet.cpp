#include <vector>
#include "DescriptorSet.h"
#include "GraphicsModule.h"
#include "Model.h"

using namespace graphics;

uint32_t GetNumberOfDescriptors(VkDescriptorPoolSize* pools, uint32_t poolsCount)
{
	uint32_t numberOfSets = 0;
	for (uint32_t i = 0; i < poolsCount; i++)
		numberOfSets += pools[i].descriptorCount;
	return numberOfSets;
}

void DescriptorSetLayout::init(DescriptorSetLayoutCreateInfo createInfo)
{
	VkDevice device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkDescriptorSetLayoutCreateInfo descriptorSetInfo{};
	descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetInfo.pBindings = createInfo.bindings;
	descriptorSetInfo.bindingCount = createInfo.count;

	if (vkCreateDescriptorSetLayout(device, &descriptorSetInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor set layout");
}

void DescriptorSetLayout::release()
{
	VkDevice device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
}


void DescriptorPool::init(VkDevice& device, VkDescriptorPoolSize* pools, uint32_t poolsCount)
{
	m_device = &device;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolsCount;
	poolInfo.pPoolSizes = pools;
	poolInfo.maxSets = GetNumberOfDescriptors(pools, poolsCount);

	if (vkCreateDescriptorPool(*m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor pool");
}


void DescriptorSet::init(uint32_t descriptorSetCount, DescriptorSetLayout layout)
{
	m_device = &GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkDescriptorPool& pool = GraphicsModule::GetInstance()->getDescriptorPool().getDescriptorPool();
	std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, layout.getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = descriptorSetCount;
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSet.resize(descriptorSetCount);
	if (vkAllocateDescriptorSets(*m_device, &allocInfo, m_descriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate descriptor sets");
}

VkWriteDescriptorSet DescriptorSet::getWriteDescriptor(uint32_t index, uint32_t binding, VkDescriptorBufferInfo* bufferInfo,
	VkDescriptorType type)
{
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = bufferInfo;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSet::getWriteDescriptor(uint32_t index, uint32_t binding, Buffer& buffer)
{
	return getWriteDescriptor(index, binding, buffer.getDescriptorBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

VkWriteDescriptorSet DescriptorSet::getWriteDescriptor(uint32_t index, uint32_t binding, TextureImage& texture)
{
	return getWriteDescriptor(index, binding, texture.getDescriptorImageInfo(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
}

VkWriteDescriptorSet DescriptorSet::getWriteDescriptor(uint32_t index, uint32_t binding, VkDescriptorImageInfo* imageInfo,
	VkDescriptorType type)
{
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo= imageInfo;
	descriptorWrite.pBufferInfo= nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	return descriptorWrite;
}


void DescriptorSet::updateDescriptors(VkWriteDescriptorSet* writes, uint32_t count)
{
	vkUpdateDescriptorSets(*m_device, count, writes, 0, nullptr);
}
