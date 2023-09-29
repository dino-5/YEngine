#pragma once
#include <vulkan/vulkan.h>

class RenderPass
{
public:
	void init(VkDevice& device, VkFormat imageFormat);
	void release()
	{
		vkDestroyRenderPass(*m_device, m_renderPass, nullptr);
	}
	VkRenderPass& getRenderPass() { return m_renderPass; }
private:
	VkRenderPass m_renderPass;
	VkDevice* m_device;
};

