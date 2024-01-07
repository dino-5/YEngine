#pragma once
#include <vulkan/vulkan.h>
#include <vector>

struct PassInfo
{
	VkFormat imageFormat;
	uint32_t numberOfColorAttachments;
	uint32_t numberOfDepthAttachments;
};

class RenderPass
{
public:
	void init(PassInfo info);
	void release()
	{
		vkDestroyRenderPass(*m_device, m_renderPass, nullptr);
	}
	VkRenderPass& getRenderPass() { return m_renderPass; }
private:
	VkRenderPass m_renderPass;
	VkDevice* m_device;
};

