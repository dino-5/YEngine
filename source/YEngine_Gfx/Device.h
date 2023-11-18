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
	void init(VkSurfaceKHR surface);
	VkPhysicalDevice& getDevice() { return m_device; }
private:
	VkPhysicalDevice m_device = nullptr;
};

class LogicalDevice
{
public:
	void init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface);
	void release()
	{
		vkDestroyDevice(m_device, nullptr);
	}
	VkDevice& getDevice() { return m_device; }
	VkQueue& getGraphicsQueue() { return m_graphicsQueue; }
	VkQueue& getPresentationQueue() { return m_presentQueue; }

private:
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkDevice m_device;
};

class Device
{
public:
	void init(VkSurfaceKHR surface)
	{
		m_physicalDevice.init(surface);
		m_logicalDevice.init(m_physicalDevice.getDevice(), surface);
	}
	void release()
	{
		m_logicalDevice.release();
	}

	PhysicalDevice& getPhysicalDevice() { return m_physicalDevice; }
	LogicalDevice& getLogicalDevice() { return m_logicalDevice; }

private:
	PhysicalDevice m_physicalDevice;
	LogicalDevice m_logicalDevice;
};

