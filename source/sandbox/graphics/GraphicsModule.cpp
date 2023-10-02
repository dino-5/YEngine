#include "GraphicsModule.h"
#include "Queue.h"
#include "../system/Window.h"

using namespace graphics;

GraphicsModule* GraphicsModule::CreateGraphicsModule(GraphicsModuleCreateInfo createInfo)
{
	ASSERT(s_module == nullptr, "failed to create one more graphics module");
	s_module = std::make_shared<GraphicsModule>(createInfo);
	s_module->swapChainInit();
	return s_module.get();
}

void GraphicsModule::ReleaseGraphicsModule()
{
	s_module->release();
}

void GraphicsModule::release()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_device.getLogicalDevice().getDevice(), m_renderFinishedSemph[i], nullptr);
		vkDestroySemaphore(m_device.getLogicalDevice().getDevice(), m_imageAvailableSemph[i], nullptr);
		vkDestroyFence(m_device.getLogicalDevice().getDevice(), m_inFlightFence[i], nullptr);
	}
	m_cmdPool.release();
	m_descriptorPool.release();
	m_swapChain.release();
	m_surface.release();
	m_device.release();
	VulkanInstance::DestroyVulkan();
}

GraphicsModule::GraphicsModule(GraphicsModuleCreateInfo createInfo)
{
	VulkanInstance::InitVulkan();
	m_surface.init();
	m_device.init(m_surface.getSurface());
	m_cmdPool.init(m_device, m_surface);
	m_descriptorPool.init(m_device.getLogicalDevice().getDevice(), createInfo.poolSizes, createInfo.numberOfPools);

}

void GraphicsModule::swapChainInit()
{
	m_swapChain.init(m_device, m_surface);
	createSyncObjects();
}

void GraphicsModule::createSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	m_imageAvailableSemph.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemph.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{

		if (vkCreateSemaphore(m_device.getLogicalDevice().getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemph[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_device.getLogicalDevice().getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemph[i]) != VK_SUCCESS ||
			vkCreateFence(m_device.getLogicalDevice().getDevice(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create syncronising objects");
	}
}

void GraphicsModule::resetFences(uint32_t frame)
{
	vkResetFences(m_device.getLogicalDevice().getDevice(), 1, &m_inFlightFence[frame]);
}

void GraphicsModule::waitForFences(uint32_t frame)
{
	vkWaitForFences(m_device.getLogicalDevice().getDevice(), 1, &m_inFlightFence[frame], VK_TRUE, UINT64_MAX);
}

int GraphicsModule::getCurrentFrameBufferIndex(uint32_t frame)
{
	return m_swapChain.getCurrentFrameBufferIndex(m_imageAvailableSemph[frame]);
}

void GraphicsModule::submit(uint32_t frame, const VkCommandBuffer* commandBuffer)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_imageAvailableSemph[frame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_renderFinishedSemph[frame];
	if (vkQueueSubmit(m_device.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo, m_inFlightFence[frame]) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer");
}

void GraphicsModule::present(uint32_t frame, uint32_t imageIndex)
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_renderFinishedSemph[frame];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_swapChain.getSwapChain();
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	vkQueuePresentKHR(m_device.getLogicalDevice().getPresentationQueue(), &presentInfo);
}

