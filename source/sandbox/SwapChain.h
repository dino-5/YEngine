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

const constexpr int MAX_FRAMES_IN_FLIGHT =2 ;

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

class SwapChain
{
public:

	void Init(GLFWwindow* window, VkPhysicalDevice& physicalDevice, VkDevice& device, VkSurfaceKHR& surface);
	void Reset()
	{
		vkDeviceWaitIdle(*m_device);
		Release();
		Create();
	}
	void Create()
	{
		QuerySwapChainProperties();
		CreateSwapChain();
		CreateSwapChainImages(*m_device);
		m_renderPass.Init(*m_device, m_surfaceFormat.format);
		CreateFrameBuffers();
	}
	float GetAspectRatio() {
		return m_extent.width / m_extent.height;
	}
	void Release()
	{
		m_renderPass.Release();
		for (auto imageView : m_imageViews) 
			vkDestroyImageView(*m_device, imageView, nullptr);

		for (auto framebuffer : m_frameBuffers)
			vkDestroyFramebuffer(*m_device, framebuffer, nullptr);

		vkDestroySwapchainKHR(*m_device, m_swapChain, nullptr);
	}

	int GetCurrentFrameBufferIndex(VkSemaphore& semaphore)
	{
		uint32_t index;
		VkResult result = vkAcquireNextImageKHR(*m_device, m_swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_isResized)
		{
			m_isResized = false;
			Reset();
			return -1;
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to acquire swap chain image");
		return index;
	}
	void SetIsResized(bool fl) { m_isResized = true; }
	VkSwapchainKHR& GetSwapChain() { return m_swapChain; }
	VkRenderPassBeginInfo GetRenderPassBeginInfo(uint32_t index);
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
	void QuerySwapChainProperties();
	void CreateSwapChainImages(VkDevice device);
	void CreateSwapChain();
	void ConfigureViewport();
	void CreateFrameBuffers();

private:
	static inline VkClearValue s_clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	static inline VkViewport s_viewport{};
	static inline VkRect2D s_scissorRect{};
	VkSwapchainKHR m_swapChain;
	VkDevice* m_device;
	VkPhysicalDevice* m_physicalDevice;
	VkSurfaceKHR* m_surface;
	GLFWwindow* m_window;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	std::vector<VkFramebuffer> m_frameBuffers;
	SwapChainSupportDetails m_swapChainSupport;
	VkPresentModeKHR m_presentMode;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkExtent2D m_extent;
	RenderPass m_renderPass;
	uint32_t m_imageCount = 0;
	bool m_isResized = false;
};

