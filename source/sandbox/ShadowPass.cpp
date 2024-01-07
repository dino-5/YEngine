#include "ShadowPass.h"
#include "YEngine_Gfx/GraphicsModule.h"

void ShadowPass::init(uint32_t width, uint32_t height)
{
	m_extent.width = width;
	m_extent.height = height;
	PassInfo passInfo
	{
		.imageFormat = VK_FORMAT_D24_UNORM_S8_UINT,
		.numberOfColorAttachments = 0,
		.numberOfDepthAttachments = 1
	};
	m_pass.init(passInfo);
	TextureCreateInfo textureInfo
	{
		.format = VK_FORMAT_D24_UNORM_S8_UINT,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
	};
	for (auto& texture : m_depthTexture)
	{
		texture.initAsDepthBuffer(width, height, textureInfo);
	}
	VkDevice device = graphics::GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();

	for (size_t i = 0; i < m_frameBuffers.size(); i++)
	{
		constexpr const uint32_t viewCount = 1;
		VkImageView views[viewCount] = { m_depthTexture[i].texture.getImageView()};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_pass.getRenderPass();
		framebufferInfo.attachmentCount = viewCount;
		framebufferInfo.pAttachments = views;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

VkRenderPassBeginInfo ShadowPass::getRenderPassBeginInfo(uint32_t index)
{
	VkRenderPassBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass = m_pass.getRenderPass();
	beginInfo.framebuffer = m_frameBuffers[index];
	beginInfo.renderArea.offset = { 0, 0 };
	beginInfo.renderArea.extent = m_extent;

	beginInfo.clearValueCount = 1;
	beginInfo.pClearValues = &m_clearDepth;

	return beginInfo;
}

void ShadowPass::release()
{
	m_pass.release();
	m_depthTexture.release();

	VkDevice device = graphics::GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	for(auto& frameBuffer: m_frameBuffers)
		vkDestroyFramebuffer(device, frameBuffer, nullptr);
}
