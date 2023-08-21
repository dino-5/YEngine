#include "Device.h"
#include "VulkanInstance.h"
#include "Queue.h"
#include <stdexcept>
#include <format>
#include <iostream>
#include <set>

VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	return properties;
}

VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& device)
{
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);
	return features;
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
	VkPhysicalDeviceProperties properties = GetPhysicalDeviceProperties(device);

	VkPhysicalDeviceFeatures features = GetPhysicalDeviceFeatures(device);

	QueueFamilyIndices indices = FindQueueFamilies(device, surface);

	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader && indices.IsComplete();
}


void PrintPhysicalDevices(const std::vector<VkPhysicalDevice>& devices)
{
	for (uint32_t i=0; i<devices.size(); ++i)
	{
		VkPhysicalDeviceProperties properties = GetPhysicalDeviceProperties(devices[i]);
		std::cout << "GPU [" <<i<<"] " <<properties.deviceName << std::endl;
	}
}

void PhysicalDevice::Init(VkSurfaceKHR surface)
{
	uint32_t deviceCount = 0;
	VkInstance instance = VulkanInstance::GetInstance();
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("no device was found");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	PrintPhysicalDevices(devices);

	// to-do: specify what device you should choose among others
	for (uint32_t i=0; i<devices.size(); ++i)
	{
		if (isPhysicalDeviceSuitable(devices[i], surface))
		{
			m_device = devices[i];
			std::cout << "Selected device [" << i << "]" << std::endl;
			break;
		}
	}
	if (m_device == nullptr)
		throw std::runtime_error("no suitable devices were found");
}

void LogicalDevice::Init(VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;

	std::set uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

	VkPhysicalDeviceFeatures deviceFeatures = GetPhysicalDeviceFeatures(physicalDevice);
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = 0;
	if (VulkanInstance::s_enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VulkanInstance::s_validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = VulkanInstance::s_validationLayers.data();
	}
	else
		deviceCreateInfo.enabledLayerCount = 0;

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device");
	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}
