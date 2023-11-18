#include <stdexcept>
#include "CommandBuffer.h"
#include "SwapChain.h"
#include "GraphicsModule.h"

using namespace graphics;

void CommandBuffer::init(uint32_t count)
{
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkCommandPool& cmdPool = GraphicsModule::GetInstance()->getCommandPool().getPool();
	m_cmdBuffer.resize(count);
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = cmdPool;
	cmdBufferAllocInfo.commandBufferCount = count;
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	if (vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, m_cmdBuffer.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer");
}

void CommandBuffer::initAsSingleTimeCmdBuffer()
{
	init(1);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginCmdBuffer(0, beginInfo);
}

void CommandBuffer::endSingleTimeCommands(uint32_t index, VkQueue queue) 
{
	endCmdBuffer(index);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_cmdBuffer[index];

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	auto cmdPool = GraphicsModule::GetInstance()->getCommandPool().getPool();
	auto device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
 
	for (uint32_t i = 0; i < m_cmdBuffer.size(); i++)
		vkFreeCommandBuffers(device, cmdPool, 1, &m_cmdBuffer[i]);
}


void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo info, uint32_t index)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	beginCmdBuffer(index, beginInfo);
	vkCmdBeginRenderPass(m_cmdBuffer[index], &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindGraphicsPipeline(VkPipeline& pipeline, uint32_t index)
{
	vkCmdBindPipeline(m_cmdBuffer[index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}


