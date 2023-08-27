#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#include "RenderPass.h"

struct GLFWwindow;

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool isSuitable() { return !formats.empty() && !presentModes.empty(); }
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

class SwapChain
{
public:

	void Init(GLFWwindow* window, VkPhysicalDevice physicalDevice, VkDevice& device, VkSurfaceKHR surface);
	void Release()
	{
		m_renderPass.Release();
		for (auto imageView : m_imageViews) 
			vkDestroyImageView(*m_device, imageView, nullptr);

		for (auto framebuffer : m_frameBuffers)
			vkDestroyFramebuffer(*m_device, framebuffer, nullptr);

		vkDestroySwapchainKHR(*m_device, m_swapChain, nullptr);
	}

	uint32_t GetCurrentFrameBufferIndex(VkSemaphore& semaphore)
	{
		uint32_t index;
		vkAcquireNextImageKHR(*m_device, m_swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &index);
		return index;
	}
	VkSwapchainKHR& GetSwapChain() { return m_swapChain; }
	VkRenderPassBeginInfo GetRenderPassInfo(uint32_t index);
	VkFormat GetFormat() { return m_surfaceFormat.format; }
	VkExtent2D GetExtent() { return m_extent; }
	VkRenderPass& GetRenderPass(){ return m_renderPass.GetRenderPass(); }
	static VkViewport& GetViewport() {
		return s_viewport;
	}

	static VkRect2D& GetScissorRect() {
		return s_scissorRect;
	}
private:
	void CreateSwapChainImages(VkDevice device);
	void CreateFrameBuffers();

private:
	static inline VkClearValue s_clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	static inline VkViewport s_viewport{};
	static inline VkRect2D s_scissorRect{};
	VkSwapchainKHR m_swapChain;
	VkDevice* m_device;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	std::vector<VkFramebuffer> m_frameBuffers;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkExtent2D m_extent;
	RenderPass m_renderPass;
};

