#include <algorithm>
#include <GLFW/glfw3.h>
#include <limits>
#include <array>
#include <stdexcept>

#include "Device.h"
#include "CommandPool.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Queue.h"
#include "../system/Window.h"



SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}


VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& format : availableFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}
	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& mode : availablePresentModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			return mode;
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
#undef max

VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	int width, height;
	Window::GetWindow()->GetFrameBufferSize(width, height);
	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};
	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
}

void SwapChain::querySwapChainProperties()
{
	m_swapChainSupport = querySwapChainSupport(*m_physicalDevice, *m_surface);

	m_presentMode = chooseSwapPresentMode(m_swapChainSupport.presentModes);
	m_surfaceFormat  = chooseSwapSurfaceFormat(m_swapChainSupport.formats);
	m_extent = chooseSwapExtent(m_swapChainSupport.capabilities);

	s_viewport.x = 0.0f;
	s_viewport.y = 0.0f;
	s_viewport.width = static_cast<float>(m_extent.width);
	s_viewport.height = static_cast<float>(m_extent.height);
	s_viewport.minDepth = 0.0f;
	s_viewport.maxDepth = 1.0f;

	s_scissorRect.offset = { 0, 0 };
	s_scissorRect.extent = m_extent;
	
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };
	 
	m_imageCount = m_swapChainSupport.capabilities.minImageCount + 1;
	if (m_swapChainSupport.capabilities.maxImageCount > 0 && m_imageCount > m_swapChainSupport.capabilities.maxImageCount)
		m_imageCount = m_swapChainSupport.capabilities.maxImageCount;
}

void SwapChain::createSwapChain()
{
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *m_surface;
	createInfo.minImageCount = m_imageCount;
	createInfo.imageFormat = m_surfaceFormat.format;
	createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
	createInfo.imageExtent = m_extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(*m_physicalDevice, *m_surface);
	uint32_t QueueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = QueueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = m_swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(*m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
		throw std::runtime_error("failed to create swap chain");
}

void SwapChain::init(Device& device, Surface& surface)
{
	m_device = &device.getLogicalDevice().getDevice();
	m_physicalDevice= &device.getPhysicalDevice().getDevice();
	m_surface = &surface.getSurface();
	create();
}

void SwapChain::createSwapChainImageViews()
{
	uint32_t imageCount;
	vkGetSwapchainImagesKHR(*m_device, m_swapChain, &imageCount, nullptr);
	m_images.resize(imageCount);
	vkGetSwapchainImagesKHR(*m_device, m_swapChain, &imageCount, m_images.data());

	m_imageViews.resize(m_images.size());
	for (size_t i = 0; i < m_images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_surfaceFormat.format;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(*m_device, &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void SwapChain::createFrameBuffers()
{
	m_frameBuffers.resize(m_imageViews.size());
	for (size_t i = 0; i < m_imageViews.size(); i++)
	{
		constexpr const uint32_t viewCount = 2;
		VkImageView views[viewCount] = { m_imageViews[i], m_depthBuffer[i].getImageView()};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass.getRenderPass();
		framebufferInfo.attachmentCount = viewCount;
		framebufferInfo.pAttachments = views;
		framebufferInfo.width = m_extent.width;
		framebufferInfo.height = m_extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*m_device, &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}


VkRenderPassBeginInfo SwapChain::getRenderPassBeginInfo(uint32_t index)
{
	VkRenderPassBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass = m_renderPass.getRenderPass();
	beginInfo.framebuffer = m_frameBuffers[index];
	beginInfo.renderArea.offset = { 0, 0 };
	beginInfo.renderArea.extent = m_extent;

	beginInfo.clearValueCount = 2;
	beginInfo.pClearValues = clearValues.data();

	return beginInfo;
}
