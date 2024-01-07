#pragma once
#include "YEngine_Gfx/RenderPass.h"
#include "YEngine_Gfx/Texture.h"

class ShadowPass
{
public:
	void init(uint32_t width, uint32_t height);
	void release();
	VkRenderPassBeginInfo getRenderPassBeginInfo(uint32_t index);
	VkDescriptorSetLayoutBinding getDescriptorSetBinding()
	{
		VkDescriptorSetLayoutBinding desc;
		desc.binding = 4;
		desc.descriptorCount = 1;
		desc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		desc.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		desc.pImmutableSamplers = nullptr;
		return desc;
	}
	TextureImage& operator[](uint32_t index) { return m_depthTexture[index]; }
public:
	VkClearValue m_clearDepth= { 1.0f, 0};
	RenderPass m_pass;
	VkExtent2D m_extent;
	FrameResources<TextureImage> m_depthTexture;
	FrameResources<VkFramebuffer> m_frameBuffers;
};

