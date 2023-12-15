#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "RenderPass.h"
#include "DescriptorSet.h"
#include "YEngine_System/system/Logger.h"

std::vector<char> ReadFile(const std::string& filename);

enum class ShaderType
{
	VERTEX,
	FRAGMENT
};
VkShaderModule CreateShader(const std::string& shaderFileName, VkDevice& device, ShaderType type);

class SwapChain;

struct GraphicsPipelineCreateInfo
{
	std::string vertexShader;
	std::string fragmentShader;
	DescriptorSetLayoutCreateInfo* layoutCreateInfo;
	uint32_t layoutCount;
};

class Shader
{
public:
	Shader() = default;
	void init(std::string path, ShaderType type);
	void release();
	VkPipelineShaderStageCreateInfo getShaderStageInfo();
private:
	std::string m_name;
	VkShaderModule m_shader;
	ShaderType m_type;
};


class GraphicsPipeline
{
public:

	void init(GraphicsPipelineCreateInfo createInfo);
	void release(bool fl = true)
	{
		// TODO: add debug define to handle this only for debug 
		if (!m_device || m_isInitialized)
		{ 
			Logger::PrintError("trying to release not initialized Graphics pipeline / or missing device");
			return;
		}
		m_vertexShader.release();
		m_fragmentShader.release();
		if(fl)
			for(auto& layout: m_descriptorSetLayouts)
				layout.release();
		vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);
		vkDestroyPipeline(*m_device, m_pipeline, nullptr);
		m_isInitialized = false;
	}
	void reload()
	{
		release(false);
		init(m_createInfo);
	}
	VkPipeline& GetPipeline() { return m_pipeline; }
	VkPipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }
	DescriptorSetLayout getDescriptorSetLayout(uint32_t index) { return m_descriptorSetLayouts[index]; }

private:
	bool m_isInitialized = false;
	GraphicsPipelineCreateInfo m_createInfo;
	Shader m_vertexShader;
	Shader m_fragmentShader;
	VkDevice* m_device;
	std::vector<DescriptorSetLayout> m_descriptorSetLayouts;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
};

