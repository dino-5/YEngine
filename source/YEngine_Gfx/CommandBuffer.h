#pragma once
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class CommandBuffer 
{
public:
	CommandBuffer() = default;
	CommandBuffer(VkCommandBuffer cmd) : m_cmdBuffer(cmd) {}
	void initAsSingleTimeCmdBuffer();
	void endSingleTimeCommands(VkQueue queue);
	void bindGraphicsPipeline(VkPipeline& pipeline);
	void beginRenderPass(VkRenderPassBeginInfo info);
	void beginCmdBuffer(VkCommandBufferBeginInfo beginInfo)
	{
		if (vkBeginCommandBuffer(m_cmdBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");
	}
	void endCmdBuffer()
	{
		if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
	void endRenderPass()
	{
		vkCmdEndRenderPass(m_cmdBuffer);
		endCmdBuffer();
	}
	void setViewport(VkViewport& viewport, uint32_t index) {
		vkCmdSetViewport(m_cmdBuffer, 0, 1, &viewport);
	}
	void setScissorRect(VkRect2D& scissor, uint32_t index) {
		vkCmdSetScissor(m_cmdBuffer, 0, 1, &scissor);
	}

	void bindVertexBuffers(VkBuffer* buffers,VkDeviceSize* offsets, uint32_t count)
	{
		vkCmdBindVertexBuffers(m_cmdBuffer, 0, count, buffers, offsets);
	}

	void bindIndexBuffers(VkBuffer& buffer)
	{
		vkCmdBindIndexBuffer(m_cmdBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void bindDescriptorSet(VkPipelineLayout pipelineLayout,
		VkDescriptorSet* descriptorSets, uint32_t numberOfSets)
	{
		vkCmdBindDescriptorSets(
			m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 
			0, numberOfSets, descriptorSets,
			0, nullptr);
	}

	void draw()
	{
		vkCmdDraw(m_cmdBuffer, 3, 1, 0, 0);
	}

	void drawIndexed(uint32_t count)
	{
		vkCmdDrawIndexed(m_cmdBuffer, count, 1, 0, 0, 0);
	}

	void copyBuffer(VkBuffer& src, VkBuffer& dst, uint32_t count, VkBufferCopy& copyRegion)
	{
		vkCmdCopyBuffer(m_cmdBuffer, src, dst, 1, &copyRegion);
	}
	
	void copyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};
		vkCmdCopyBufferToImage(m_cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void pipelineBarrier(VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkImageMemoryBarrier& barrier)
	{
		vkCmdPipelineBarrier(m_cmdBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void reset()
	{
		vkResetCommandBuffer(m_cmdBuffer, 0);
	}
	VkCommandBuffer& getNative() { return m_cmdBuffer; }

private:
	VkCommandBuffer m_cmdBuffer;
};

