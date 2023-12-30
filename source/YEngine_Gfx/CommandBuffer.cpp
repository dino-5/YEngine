#include <stdexcept>
#include "CommandBuffer.h"
#include "SwapChain.h"
#include "GraphicsModule.h"

using namespace graphics;

void CommandBuffer::initAsSingleTimeCmdBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginCmdBuffer(beginInfo);
}

void CommandBuffer::endSingleTimeCommands(VkQueue queue) 
{
	endCmdBuffer();
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_cmdBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	auto cmdPool = GraphicsModule::GetInstance()->getCommandPool().getPool();
	auto device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
 
	vkFreeCommandBuffers(device, cmdPool, 1, &m_cmdBuffer);
}


void CommandBuffer::beginRenderPass(VkRenderPassBeginInfo info)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	beginCmdBuffer(beginInfo);
	vkCmdBeginRenderPass(m_cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindGraphicsPipeline(VkPipeline& pipeline)
{
	vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}


