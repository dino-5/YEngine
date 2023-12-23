#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "RenderPass.h"
#include "Texture.h"
#include "gfx_common.h"

struct GLFWwindow;
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	bool isSuitable() { return !formats.empty() && !presentModes.empty(); }
};


SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

class Device;
class Surface;
class CommandPool;

class SwapChain
{
public:

	void init(Device& device, Surface& surface);
	void reset()
	{
		vkDeviceWaitIdle(*m_device);
		release();
		create();
	}
	void create()
	{
		querySwapChainProperties();
		createSwapChain();
		createSwapChainImageViews();
		m_renderPass.init(*m_device, m_surfaceFormat.format);
		TextureCreateInfo depthInfo{
			.format = VK_FORMAT_D24_UNORM_S8_UINT,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT
		};
		for(int i=0; i<3; i++)
			m_depthBuffer[i].initAsDepthBuffer(s_viewport.width, s_viewport.height, depthInfo);
		createFrameBuffers();
	}
	float getAspectRatio() {
		return m_extent.width / m_extent.height;
	}
	void release()
	{
		for(auto& depthBuffer : m_depthBuffer)
			depthBuffer.release();

		m_renderPass.release();
		for (auto imageView : m_imageViews) 
			vkDestroyImageView(*m_device, imageView, nullptr);

		for (auto framebuffer : m_frameBuffers)
			vkDestroyFramebuffer(*m_device, framebuffer, nullptr);

		vkDestroySwapchainKHR(*m_device, m_swapChain, nullptr);
	}

	int getCurrentFrameBufferIndex(VkSemaphore& semaphore)
	{
		uint32_t index;
		VkResult result = vkAcquireNextImageKHR(*m_device, m_swapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_isResized)
		{
			m_isResized = false;
			reset();
			return -1;
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to acquire swap chain image");
		return index;
	}
	void setIsResized(bool fl) { m_isResized = true; }
	VkSwapchainKHR& getSwapChain() { return m_swapChain; }
	VkRenderPassBeginInfo getRenderPassBeginInfo(uint32_t index);
	VkFormat getFormat() { return m_surfaceFormat.format; }
	VkExtent2D getExtent() { return m_extent; }
	VkRenderPass& getRenderPass(){ return m_renderPass.getRenderPass(); }
	uint32_t getImageCount() { return m_imageCount; }
	static VkViewport& getViewport() {
		return s_viewport;
	}

	static VkRect2D& getScissorRect() {
		return s_scissorRect;
	}
private:
	void querySwapChainProperties();
	void createSwapChainImageViews();
	void createSwapChain();
	void createFrameBuffers();

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

