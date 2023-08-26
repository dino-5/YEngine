#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "RenderPass.h"

std::vector<char> ReadFile(const std::string& filename);
//std::vector<uint32_t> ReadFile(const std::string& filename);
VkShaderModule CreateShader(const std::string& shaderFileName, VkDevice& device);

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};

class GraphicsPipeline
{
public:
	void Init(VkDevice& device, VkExtent2D extent, VkFormat imageFormat);
	void Release()
	{
		vkDestroyShaderModule(*m_device, m_vertexShader, nullptr);
		vkDestroyShaderModule(*m_device, m_fragmentShader, nullptr);

		vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(*m_device, m_pipeline, nullptr);
		m_renderPass.Release();
	}

private:
	VkShaderModule m_vertexShader;
	VkShaderModule m_fragmentShader;
	VkDevice* m_device;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
	RenderPass m_renderPass;
};

