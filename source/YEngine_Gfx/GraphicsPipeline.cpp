#include <fstream>
#include <iostream>
#include <shaderc/shaderc.h>

#include "YEngine_System/Geometry.h"
#include "YEngine_System/system/Logger.h"
#include "YEngine_System/system/Filesystem.h"

#include "GraphicsPipeline.h"
#include "math.h"
#include "SwapChain.h"
#include "GraphicsModule.h"

using namespace graphics;

std::vector<char> ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("failed to open file!");
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

uint32_t GetNumberDividedBy4(uint32_t number)
{
	return number + (sizeof(uint32_t) - number % 4) * (number % 4 > 0);
}


VkShaderModule CreateShader(const std::string& shaderFileName, VkDevice& device, ShaderType type)
{
    shaderc_compiler_t compiler = shaderc_compiler_initialize();
	std::string sourceCode = File(shaderFileName).readFile();

    shaderc_compilation_result_t result = shaderc_compile_into_spv(
		compiler, sourceCode.c_str(), sourceCode.length(),
	    type == ShaderType::VERTEX ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader,
		shaderFileName.c_str(), "main", nullptr
	);

	VkShaderModule m_shader;
	VkShaderModuleCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderc_result_get_length(result);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result));
	createInfo.flags = 0;
	createInfo.pNext = nullptr;

	if (vkCreateShaderModule(device, &createInfo, nullptr, &m_shader) != VK_SUCCESS)
		std::runtime_error("failed to create shader");

    shaderc_result_release(result);
    shaderc_compiler_release(compiler);
	return m_shader;
}

VkShaderStageFlagBits castToNativeEnum(ShaderType type)
{
	switch (type)
	{
	case ShaderType::VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case ShaderType::FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	default:
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
}

VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderModule shaderModule, ShaderType type)
{
	VkPipelineShaderStageCreateInfo ShaderStageInfo{};
	ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStageInfo.stage = castToNativeEnum(type);
	ShaderStageInfo.module = shaderModule;
	ShaderStageInfo.pName = "main";
	ShaderStageInfo.flags = 0;
	ShaderStageInfo.pNext = nullptr;
	return ShaderStageInfo;
}

void Shader::init(std::string path, ShaderType type)
{
	m_name = path;
	m_type = type;
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	m_shader = CreateShader(path, device, type);
}

VkPipelineShaderStageCreateInfo Shader::getShaderStageInfo()
{
	return CreateShaderStageInfo(m_shader, m_type);
}

void Shader::release()
{
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	vkDestroyShaderModule(device, m_shader, nullptr);
}

void GraphicsPipeline::init(GraphicsPipelineCreateInfo createInfo)
{
	m_createInfo = createInfo;
	m_device = &GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	m_vertexShader.init(createInfo.vertexShader, ShaderType::VERTEX);
	m_fragmentShader.init(createInfo.fragmentShader, ShaderType::FRAGMENT);
	if (!m_descriptorSetLayouts.size())
	{
		m_descriptorSetLayouts.resize(createInfo.layoutCount);
		for (uint32_t i = 0; i < createInfo.layoutCount; ++i)
			m_descriptorSetLayouts[i].init(createInfo.layoutCreateInfo[i]);
	}
	
	VkPipelineShaderStageCreateInfo shaderStages[] = { m_vertexShader.getShaderStageInfo(), m_fragmentShader.getShaderStageInfo() };

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	auto bindingDescription = Geometry::Vertex::GetBindingDescription();
	auto attributesDescription = Geometry::Vertex::GetAttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributesDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributesDescription.data(); // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = createInfo.layoutCount; // Optional
	pipelineLayoutInfo.pSetLayouts =
		reinterpret_cast<VkDescriptorSetLayout*>(m_descriptorSetLayouts.data()); // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(*m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) 
		throw std::runtime_error("failed to create pipeline layout!");

	VkPipelineDepthStencilStateCreateInfo depthStencilState{};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_TRUE;
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilState.depthBoundsTestEnable = VK_TRUE;
	depthStencilState.minDepthBounds = 0.0f;
	depthStencilState.maxDepthBounds = 1.0f;

	depthStencilState.stencilTestEnable = VK_FALSE;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.pDynamicState = &dynamicState;
	pipelineCreateInfo.layout = m_pipelineLayout;
	pipelineCreateInfo.renderPass = GraphicsModule::GetInstance()->getSwapChain().getRenderPass();
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineCreateInfo.basePipelineIndex = -1; // Optional
	
	if (vkCreateGraphicsPipelines(*m_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline");
}
