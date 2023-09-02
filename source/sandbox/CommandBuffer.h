#pragma once
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class CommandBuffer 
{
public:
	void Init(VkDevice& device, VkCommandPool& pool);
	void BindGraphicsPipeline(VkPipeline& pipeline, uint32_t index);
	void BeginRenderPass(VkRenderPassBeginInfo info, uint32_t index);
	void EndRenderPass(uint32_t index)
	{
		vkCmdEndRenderPass(m_cmdBuffer[index]);
		if (vkEndCommandBuffer(m_cmdBuffer[index]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
	void SetViewport(VkViewport& viewport, uint32_t index) {
		vkCmdSetViewport(m_cmdBuffer[index], 0, 1, &viewport);
	}
	void SetScissorRect(VkRect2D& scissor, uint32_t index) {
		vkCmdSetScissor(m_cmdBuffer[index], 0, 1, &scissor);
	}

	void BindVertexBuffers(uint32_t index, VkBuffer* buffers,VkDeviceSize* offsets, uint32_t count)
	{
		vkCmdBindVertexBuffers(m_cmdBuffer[index], 0, count, buffers, offsets);
	}

	void BindIndexBuffers(uint32_t index, VkBuffer& buffer)
	{
		vkCmdBindIndexBuffer(m_cmdBuffer[index], buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void BindDescriptorSet(uint32_t index, VkPipelineLayout pipelineLayout, VkDescriptorSet& descriptor)
	{
		vkCmdBindDescriptorSets(m_cmdBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptor, 0, nullptr);
	}

	void Draw(uint32_t index)
	{
		vkCmdDraw(m_cmdBuffer[index], 3, 1, 0, 0);
	}

	void DrawIndexed(uint32_t index, uint32_t count)
	{
		vkCmdDrawIndexed(m_cmdBuffer[index], count, 1, 0, 0, 0);
	}

	void Reset(uint32_t index)
	{
		vkResetCommandBuffer(m_cmdBuffer[index], 0);
	}
	VkCommandBuffer& GetCmdBuffer(uint32_t index) { return m_cmdBuffer[index]; }
private:
	std::vector<VkCommandBuffer> m_cmdBuffer;
	VkDevice* m_device;
};

