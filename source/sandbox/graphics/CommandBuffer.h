#pragma once
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class CommandBuffer 
{
public:
	void init( uint32_t count);
	void initAsSingleTimeCmdBuffer();
	void endSingleTimeCommands(uint32_t index, VkQueue queue);
	void bindGraphicsPipeline(VkPipeline& pipeline, uint32_t index);
	void beginRenderPass(VkRenderPassBeginInfo info, uint32_t index);
	void beginCmdBuffer(uint32_t index, VkCommandBufferBeginInfo beginInfo)
	{
		if (vkBeginCommandBuffer(m_cmdBuffer[index], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");
	}
	void endCmdBuffer(uint32_t index)
	{
		if (vkEndCommandBuffer(m_cmdBuffer[index]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
	void endRenderPass(uint32_t index)
	{
		vkCmdEndRenderPass(m_cmdBuffer[index]);
		endCmdBuffer(index);
	}
	void setViewport(VkViewport& viewport, uint32_t index) {
		vkCmdSetViewport(m_cmdBuffer[index], 0, 1, &viewport);
	}
	void setScissorRect(VkRect2D& scissor, uint32_t index) {
		vkCmdSetScissor(m_cmdBuffer[index], 0, 1, &scissor);
	}

	void bindVertexBuffers(uint32_t index, VkBuffer* buffers,VkDeviceSize* offsets, uint32_t count)
	{
		vkCmdBindVertexBuffers(m_cmdBuffer[index], 0, count, buffers, offsets);
	}

	void bindIndexBuffers(uint32_t index, VkBuffer& buffer)
	{
		vkCmdBindIndexBuffer(m_cmdBuffer[index], buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void bindDescriptorSet(uint32_t index, VkPipelineLayout pipelineLayout, VkDescriptorSet& descriptor)
	{
		vkCmdBindDescriptorSets(m_cmdBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptor, 0, nullptr);
	}

	void draw(uint32_t index)
	{
		vkCmdDraw(m_cmdBuffer[index], 3, 1, 0, 0);
	}

	void drawIndexed(uint32_t index, uint32_t count)
	{
		vkCmdDrawIndexed(m_cmdBuffer[index], count, 1, 0, 0, 0);
	}

	void copyBuffer(uint32_t index, VkBuffer& src, VkBuffer& dst, uint32_t count, VkBufferCopy& copyRegion)
	{
		vkCmdCopyBuffer(m_cmdBuffer[index], src, dst, 1, &copyRegion);
	}
	
	void copyBufferToImage(uint32_t index, VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
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
		vkCmdCopyBufferToImage(m_cmdBuffer[index], buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void pipelineBarrier(uint32_t index, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
		VkImageMemoryBarrier& barrier)
	{
		vkCmdPipelineBarrier(m_cmdBuffer[index],
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void reset(uint32_t index)
	{
		vkResetCommandBuffer(m_cmdBuffer[index], 0);
	}
	VkCommandBuffer& getCmdBuffer(uint32_t index) { return m_cmdBuffer[index]; }

private:
	std::vector<VkCommandBuffer> m_cmdBuffer;
	VkDevice* m_device;
	VkCommandPool* m_cmdPool;
};

