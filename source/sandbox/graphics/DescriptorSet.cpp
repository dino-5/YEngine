#include <vector>
#include "DescriptorSet.h"

uint32_t GetNumberOfDescriptors(VkDescriptorPoolSize* pools, uint32_t poolsCount)
{
	uint32_t numberOfSets = 0;
	for (uint32_t i = 0; i < poolsCount; i++)
		numberOfSets += pools[i].descriptorCount;
	return numberOfSets;
}

void DescriptorPool::Init(VkDevice& device, VkDescriptorPoolSize* pools, uint32_t poolsCount)
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


void DescriptorSet::Init(VkDevice& device, uint32_t descriptorSetCount, VkDescriptorSetLayout layout, VkDescriptorPool& pool)
{
	m_device = &device;
	std::vector<VkDescriptorSetLayout> layouts(descriptorSetCount, layout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = descriptorSetCount;
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSet.resize(descriptorSetCount);
	if (vkAllocateDescriptorSets(*m_device, &allocInfo, m_descriptorSet.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate descriptor sets");
}

VkWriteDescriptorSet DescriptorSet::GetWriteDescriptor(uint32_t index, VkDescriptorBufferInfo bufferInfo, VkDescriptorType type)
{
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet[index];
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSet::GetWriteDescriptor(uint32_t index, VkDescriptorImageInfo imageInfo, VkDescriptorType type)
{
	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet[index];
	descriptorWrite.dstBinding = 1;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo= &imageInfo;
	descriptorWrite.pBufferInfo= nullptr; // Optional
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	return descriptorWrite;
}


void DescriptorSet::UpdateDescriptors(VkWriteDescriptorSet* writes, uint32_t count)
{
	vkUpdateDescriptorSets(*m_device, count, writes, 0, nullptr);
}
