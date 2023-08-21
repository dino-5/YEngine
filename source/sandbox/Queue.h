#pragma once
#include <vulkan/vulkan.h>
#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily{};
	std::optional<uint32_t> presentFamily{};
	bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR surface);

class Queue
{
public:
private:
};

