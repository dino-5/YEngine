#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "RenderPass.h"

std::vector<char> ReadFile(const std::string& filename);
VkShaderModule CreateShader(const std::string& shaderFileName, VkDevice& device);

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};

class SwapChain;

class GraphicsPipeline
{
public:
	void init(VkDescriptorSetLayout* layouts, uint32_t layoutsCount);
	void Release()
	{
		vkDestroyShaderModule(*m_device, m_vertexShader, nullptr);
		vkDestroyShaderModule(*m_device, m_fragmentShader, nullptr);

		vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(*m_device, m_pipeline, nullptr);
	}
	VkPipeline& GetPipeline() { return m_pipeline; }
	VkPipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }

private:
	VkShaderModule m_vertexShader;
	VkShaderModule m_fragmentShader;
	VkDevice* m_device;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
};

