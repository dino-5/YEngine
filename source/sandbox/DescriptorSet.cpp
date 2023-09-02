#include <vector>
#include "DescriptorSet.h"

void DescriptorPool::Init(VkDevice& device, uint32_t descriptorCount, VkDescriptorType descriptorPoolType)
{
	m_device = &device;
	VkDescriptorPoolSize poolSize{};
	poolSize.descriptorCount = descriptorCount;
	poolSize.type = descriptorPoolType;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = descriptorCount;

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

void DescriptorSet::UpdateDescriptors(VkBuffer& buffer, uint32_t sizeOfBuffer, uint32_t index, VkDescriptorType type)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeOfBuffer;
	
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
	vkUpdateDescriptorSets(*m_device, 1, &descriptorWrite, 0, nullptr);

}
