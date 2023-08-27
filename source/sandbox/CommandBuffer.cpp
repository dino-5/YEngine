#include <stdexcept>
#include "CommandBuffer.h"

void CommandBuffer::Init(VkDevice& device, VkCommandPool& pool)
{
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = pool;
	cmdBufferAllocInfo.commandBufferCount = 1;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, &m_cmdBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
}


void CommandBuffer::BeginRenderPass(VkRenderPassBeginInfo info)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_cmdBuffer, &beginInfo) != VK_SUCCESS) 
		throw std::runtime_error("failed to begin recording command buffer!");

	vkCmdBeginRenderPass(m_cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindGraphicsPipeline(VkPipeline& pipeline)
{
	vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}


