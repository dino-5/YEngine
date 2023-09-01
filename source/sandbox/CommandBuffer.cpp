#include <stdexcept>
#include "CommandBuffer.h"
#include "SwapChain.h"

void CommandBuffer::Init(VkDevice& device, VkCommandPool& pool)
{
	m_cmdBuffer.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = pool;
	cmdBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, m_cmdBuffer.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
}


void CommandBuffer::BeginRenderPass(VkRenderPassBeginInfo info, uint32_t index)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_cmdBuffer[index], &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");

	vkCmdBeginRenderPass(m_cmdBuffer[index], &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindGraphicsPipeline(VkPipeline& pipeline, uint32_t index)
{
	vkCmdBindPipeline(m_cmdBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}


