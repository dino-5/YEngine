#pragma once
#include <vulkan/vulkan.h>
#include <vector>

VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& device);
VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& device);

class PhysicalDevice
{
public:
	static const inline std::vector<const char*> s_requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	void Init(VkSurfaceKHR surface);
	VkPhysicalDevice& GetDevice() { return m_device; }
private:
	VkPhysicalDevice m_device = nullptr;
};

class LogicalDevice
{
public:
	void Init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);
	void Release()
	{

		vkDestroyDevice(m_device, nullptr);
	}
	VkDevice& GetDevice() { return m_device; }
	VkQueue& GetGraphicsQueue() { return m_graphicsQueue; }
	VkQueue& GetPresentationQueue() { return m_presentQueue; }

private:
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkDevice m_device;
};

