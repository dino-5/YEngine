#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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
		for (auto imageView : m_imageViews) 
			vkDestroyImageView(*m_device, imageView, nullptr);

		vkDestroySwapchainKHR(*m_device, m_swapChain, nullptr);
	}

	VkFormat GetFormat() { return m_surfaceFormat.format; }
	VkExtent2D GetExtent() { return m_extent; }
private:
	void CreateSwapChainImages(VkDevice device);

private:
	VkSwapchainKHR m_swapChain;
	VkDevice* m_device;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	VkSurfaceFormatKHR m_surfaceFormat;
	VkExtent2D m_extent;
};

