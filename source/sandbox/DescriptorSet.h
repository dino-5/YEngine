#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "common.h"

class DescriptorSetLayout
{
public:
	DesctructorVulkanObject(DescriptorSetLayout)

	void Init(VkDevice& device, VkDescriptorSetLayoutBinding* bindings, uint32_t count)
	{
		VulkanObjectInitialized();
		m_device = &device;
		VkDescriptorSetLayoutCreateInfo descriptorSetInfo{};
		descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetInfo.bindingCount = count;
		descriptorSetInfo.pBindings = bindings;

		if (vkCreateDescriptorSetLayout(device, &descriptorSetInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout");
	}

	void Release()
	{
		VulkanObjectReleased();
		vkDestroyDescriptorSetLayout(*m_device, m_descriptorSetLayout, nullptr);
	}
	VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_descriptorSetLayout; }
		
private:
	VkDevice* m_device;
	VkDescriptorSetLayout m_descriptorSetLayout;
};

class DescriptorPool
{
public:
	void Init(VkDevice& device, uint32_t descriptorCount, VkDescriptorType descriptorPoolType);
	void Release()
	{
		vkDestroyDescriptorPool(*m_device, m_descriptorPool, nullptr);
	}
	VkDescriptorPool& GetDescriptorPool() { return m_descriptorPool; }
private:
	VkDevice* m_device;
	VkDescriptorPool m_descriptorPool;
};

class DescriptorSet
{
public:
	void Init(VkDevice& device, uint32_t descriptorSetCount, VkDescriptorSetLayout layout, VkDescriptorPool& pool);
	void UpdateDescriptors(VkBuffer& buffer, uint32_t sizeOfBuffer, uint32_t index, VkDescriptorType type);
	void Release()
	{}
	VkDescriptorSet& GetDescriptorSet(uint32_t index) { return m_descriptorSet[index]; }
private:
	VkDevice* m_device;
	std::vector<VkDescriptorSet> m_descriptorSet;
};
