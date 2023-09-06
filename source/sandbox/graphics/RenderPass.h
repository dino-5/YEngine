#pragma once
#include <vulkan/vulkan.h>

class RenderPass
{
public:
	void Init(VkDevice& device, VkFormat imageFormat);
	void Release()
	{
		vkDestroyRenderPass(*m_device, m_renderPass, nullptr);
	}
	VkRenderPass& GetRenderPass() { return m_renderPass; }
private:
	VkRenderPass m_renderPass;
	VkDevice* m_device;
};

