#pragma once
#include<vulkan/vulkan.h>
#include <stdexcept>

class CommandBuffer 
{
public:
	void Init(VkDevice& device, VkCommandPool& pool);
	void BindGraphicsPipeline(VkPipeline& pipeline);
	void BeginRenderPass(VkRenderPassBeginInfo info);
	void EndRenderPass()
	{
		vkCmdEndRenderPass(m_cmdBuffer);
		if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
	void SetViewport(VkViewport& viewport) {
		vkCmdSetViewport(m_cmdBuffer, 0, 1, &viewport);
	}
	void SetScissorRect(VkRect2D& scissor) {
		vkCmdSetScissor(m_cmdBuffer, 0, 1, &scissor);
	}
	void Draw()
	{
		vkCmdDraw(m_cmdBuffer, 3, 1, 0, 0);
	}
	void Reset()
	{
		vkResetCommandBuffer(m_cmdBuffer, 0);
	}
	VkCommandBuffer& GetCmdBuffer() { return m_cmdBuffer; }
private:
	VkCommandBuffer m_cmdBuffer;
	VkDevice* m_device;
};

