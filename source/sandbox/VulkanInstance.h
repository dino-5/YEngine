#pragma once
#include<vulkan/vulkan.h>
#include <vector>

class VulkanInstance
{
public:
	static void InitVulkan();
	static void DestroyVulkan();
private:
	static void CreateInstance();
	static void DestroyInstance();
	static void SetupDebugMessenger();
	static void DestroyDebugMessenger();
	static bool CheckValidationLayerSupport();
	static std::vector<const char*> getRequiredExtensions();
	
	static const bool enableValidationLayers;
	static VkInstance instance;
	static inline VkDebugUtilsMessengerEXT debugMessenger;
	static inline const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
};

