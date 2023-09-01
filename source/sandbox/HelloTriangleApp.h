#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "VulkanInstance.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

class HelloTriangleApplication 
{
public:
	static inline HelloTriangleApplication* m_instance = nullptr;
	HelloTriangleApplication()
	{
		if (m_instance == nullptr)
			m_instance = this;
		else
			throw std::runtime_error("tried to create one more app");
	}
	static const constexpr uint32_t WIDTH = 800;
	static const constexpr uint32_t HEIGHT = 600;
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void SetIsResized(bool fl)
	{
		m_swapChain.SetIsResized(fl);
	}

private:
	void initWindow()
	{

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
		glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
	}
	void initVulkan() {
		VulkanInstance::InitVulkan();
		m_surface.Init(window);
		m_physicalDevice.Init(m_surface.GetSurface());
		m_logicalDevice.Init(m_physicalDevice.GetDevice(), m_surface.GetSurface());
		m_swapChain.Init(window, m_physicalDevice.GetDevice(), m_logicalDevice.GetDevice(), m_surface.GetSurface());
		m_pipeline.Init(m_logicalDevice.GetDevice(), m_swapChain.GetExtent(), m_swapChain.GetFormat(), m_swapChain.GetRenderPass());
		m_pool.Init(m_physicalDevice.GetDevice(), m_surface.GetSurface(), m_logicalDevice.GetDevice());
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_cmdBuffer.Init(m_logicalDevice.GetDevice(), m_pool.GetPool());
		createSyncObjects();
	}

	void createSyncObjects()
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

			if (vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemph[i]) ||
				vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemph[i]) ||
				vkCreateFence(m_logicalDevice.GetDevice(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create syncronising objects");
		}
	}

	void mainLoop() 
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(m_logicalDevice.GetDevice());
	}

	void drawFrame()
	{
		vkWaitForFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);
		int imageIndexT = m_swapChain.GetCurrentFrameBufferIndex(m_imageAvailableSemph[m_currentFrame]);
		if (imageIndexT == -1)
			return;
		uint32_t imageIndex = static_cast<uint32_t>(imageIndexT);
		vkResetFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFence[m_currentFrame]);
		m_cmdBuffer.Reset(m_currentFrame);
		m_cmdBuffer.BeginRenderPass(m_swapChain.GetRenderPassInfo(imageIndex), m_currentFrame);
		m_cmdBuffer.BindGraphicsPipeline(m_pipeline.GetPipeline(), m_currentFrame);
		m_cmdBuffer.SetViewport(SwapChain::GetViewport(), m_currentFrame);
		m_cmdBuffer.SetScissorRect(SwapChain::GetScissorRect(), m_currentFrame);
		m_cmdBuffer.Draw(m_currentFrame);
		m_cmdBuffer.EndRenderPass(m_currentFrame);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemph[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_cmdBuffer.GetCmdBuffer(m_currentFrame);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_renderFinishedSemph[m_currentFrame];
		if (vkQueueSubmit(m_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_renderFinishedSemph[m_currentFrame];
		VkSwapchainKHR swapChains[] = { m_swapChain.GetSwapChain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		vkQueuePresentKHR(m_logicalDevice.GetPresentationQueue(), &presentInfo);
		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void cleanup() {
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			vkDestroySemaphore(m_logicalDevice.GetDevice(), m_renderFinishedSemph[i], nullptr);
			vkDestroySemaphore(m_logicalDevice.GetDevice(), m_imageAvailableSemph[i], nullptr);
			vkDestroyFence(m_logicalDevice.GetDevice(), m_inFlightFence[i], nullptr);
		}
		m_pool.Release();
		m_pipeline.Release();
		m_swapChain.Release();
		m_surface.Release();
		m_logicalDevice.Release();
		VulkanInstance::DestroyVulkan();
		glfwDestroyWindow(window); 
		glfwTerminate();
	}


private:
	GLFWwindow* window;
	PhysicalDevice m_physicalDevice;
	LogicalDevice m_logicalDevice;
	Surface m_surface;
	SwapChain m_swapChain;
	GraphicsPipeline m_pipeline;
	CommandPool m_pool;
	CommandBuffer m_cmdBuffer;
	std::vector<VkSemaphore> m_imageAvailableSemph;
	std::vector<VkSemaphore> m_renderFinishedSemph;
	std::vector<VkFence> m_inFlightFence;
	uint32_t m_currentFrame = 0;
};

