#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "RenderPass.h"
#include "DescriptorSet.h"

std::vector<char> ReadFile(const std::string& filename);
VkShaderModule CreateShader(const std::string& shaderFileName, VkDevice& device);

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};

class SwapChain;

struct GraphicsPipelineCreateInfo
{
	std::string vertexShader;
	std::string fragmentShader;
	DescriptorSetLayoutCreateInfo* layoutCreateInfo;
	uint32_t layoutCount;
};

class GraphicsPipeline
{
public:
	void init(GraphicsPipelineCreateInfo createInfo);
	void release()
	{
		vkDestroyShaderModule(*m_device, m_vertexShader, nullptr);
		vkDestroyShaderModule(*m_device, m_fragmentShader, nullptr);
		for(auto& layout: m_descriptorSetLayouts)
			layout.release();
		vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(*m_device, m_pipeline, nullptr);
	}
	VkPipeline& GetPipeline() { return m_pipeline; }
	VkPipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }
	DescriptorSetLayout getDescriptorSetLayout(uint32_t index) { return m_descriptorSetLayouts[index]; }

private:
	VkShaderModule m_vertexShader;
	VkShaderModule m_fragmentShader;
	VkDevice* m_device;
	std::vector<DescriptorSetLayout> m_descriptorSetLayouts;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
};

