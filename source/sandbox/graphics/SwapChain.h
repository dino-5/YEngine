#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "RenderPass.h"
#include "Texture.h"

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

	void Init(VkPhysicalDevice& physicalDevice, VkDevice& device, VkSurfaceKHR& surface, VkQueue& queue,
		VkCommandPool& cmdPool);
	void Reset(VkQueue& queue, VkCommandPool& cmdPool)
	{
		vkDeviceWaitIdle(*m_device);
		Release();
		Create(queue, cmdPool);
	}
	void Create(VkQueue& queue, VkCommandPool& cmdPool)
	{
		QuerySwapChainProperties();
		CreateSwapChain();
		CreateSwapChainImageViews();
		m_renderPass.Init(*m_device, m_surfaceFormat.format);
		TextureCreateInfo depthInfo{
			.format = VK_FORMAT_D24_UNORM_S8_UINT,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT
		};
		for(int i=0; i<3; i++)
			m_depthBuffer[i].InitAsDepthBuffer(*m_device, *m_physicalDevice, cmdPool, queue,
				s_viewport.width, s_viewport.height, depthInfo);
		CreateFrameBuffers();
	}
	float GetAspectRatio() {
		return m_extent.width / m_extent.height;
	}
	void Release()
	{
		for(auto& depthBuffer : m_depthBuffer)
			depthBuffer.Release();

		m_renderPass.Release();
		for (auto imageView : m_imageViews) 
			vkDestroyImageView(*m_device, imageView, nullptr);

		for (auto framebuffer : m_frameBuffers)
			vkDestroyFramebuffer(*m_device, framebuffer, nullptr);

		vkDestroySwapchainKHR(*m_device, m_swapChain, nullptr);
	}

	int GetCurrentFrameBufferIndex(VkSemaphore& semaphore, VkQueue& queue, VkCommandPool& cmdPool)
	{
		uint32_t index;
		VkResult result = vkAcquireNextImageKHR(*m_device, m_swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_isResized)
		{
			m_isResized = false;
			Reset(queue, cmdPool);
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
	void CreateSwapChainImageViews();
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
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	std::vector<VkFramebuffer> m_frameBuffers;
	std::array<Texture, 3> m_depthBuffer;
	std::array<VkClearValue, 2> clearValues{};
	SwapChainSupportDetails m_swapChainSupport;
	VkPresentModeKHR m_presentMode;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkExtent2D m_extent;
	RenderPass m_renderPass;
	uint32_t m_imageCount = 0;
	bool m_isResized = false;
};

