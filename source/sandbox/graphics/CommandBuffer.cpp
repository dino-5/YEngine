#include <stdexcept>
#include "CommandBuffer.h"
#include "SwapChain.h"

void CommandBuffer::Init(VkDevice& device, VkCommandPool& pool, uint32_t count)
{
	m_device = &device;
	m_cmdPool = &pool;
	m_cmdBuffer.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = pool;
	cmdBufferAllocInfo.commandBufferCount = count;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, m_cmdBuffer.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
}

void CommandBuffer::InitAsSingleTimeCmdBuffer(VkDevice& device, VkCommandPool& pool)
{
	Init(device, pool, 1);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginCmdBuffer(0, beginInfo);
}

void CommandBuffer::EndSingleTimeCommands(uint32_t index, VkQueue queue) 
{
	EndCmdBuffer(index);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_cmdBuffer[index];

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	for (uint32_t i = 0; i < m_cmdBuffer.size(); i++)
		vkFreeCommandBuffers(*m_device, *m_cmdPool, 1, &m_cmdBuffer[i]);
}


void CommandBuffer::BeginRenderPass(VkRenderPassBeginInfo info, uint32_t index)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	BeginCmdBuffer(index, beginInfo);
	vkCmdBeginRenderPass(m_cmdBuffer[index], &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::BindGraphicsPipeline(VkPipeline& pipeline, uint32_t index)
{
	vkCmdBindPipeline(m_cmdBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}


