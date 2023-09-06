#pragma once
#include<vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class CommandBuffer 
{
public:
	void Init(VkDevice& device, VkCommandPool& pool, uint32_t count);
	void InitAsSingleTimeCmdBuffer(VkDevice& device, VkCommandPool& pool);
	void EndSingleTimeCommands(uint32_t index, VkQueue queue);
	void BindGraphicsPipeline(VkPipeline& pipeline, uint32_t index);
	void BeginRenderPass(VkRenderPassBeginInfo info, uint32_t index);
	void BeginCmdBuffer(uint32_t index, VkCommandBufferBeginInfo beginInfo)
	{
		if (vkBeginCommandBuffer(m_cmdBuffer[index], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");
	}
	void EndCmdBuffer(uint32_t index)
	{
		if (vkEndCommandBuffer(m_cmdBuffer[index]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
	void EndRenderPass(uint32_t index)
	{
		vkCmdEndRenderPass(m_cmdBuffer[index]);
		EndCmdBuffer(index);
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

	void CopyBuffer(uint32_t index, VkBuffer& src, VkBuffer& dst, uint32_t count, VkBufferCopy& copyRegion)
	{
		vkCmdCopyBuffer(m_cmdBuffer[index], src, dst, 1, &copyRegion);
	}
	
	void CopyBufferToImage(uint32_t index, VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
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

	void PipelineBarrier(uint32_t index, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
		VkImageMemoryBarrier& barrier)
	{
		vkCmdPipelineBarrier(m_cmdBuffer[index],
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void Reset(uint32_t index)
	{
		vkResetCommandBuffer(m_cmdBuffer[index], 0);
	}
	VkCommandBuffer& GetCmdBuffer(uint32_t index) { return m_cmdBuffer[index]; }
private:
	std::vector<VkCommandBuffer> m_cmdBuffer;
	VkDevice* m_device;
	VkCommandPool* m_cmdPool;
};

