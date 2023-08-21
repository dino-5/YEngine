#pragma once
#include<vulkan/vulkan.h>
#include <vector>

class VulkanInstance
{
public:
	static void InitVulkan();
	static void DestroyVulkan();
	static VkInstance GetInstance() { return instance; }
	static const bool s_enableValidationLayers;
	static inline const std::vector<const char*> s_validationLayers = { "VK_LAYER_KHRONOS_validation" };
private:
	static void CreateInstance();
	static void DestroyInstance();
	static void SetupDebugMessenger();
	static void DestroyDebugMessenger();
	static bool CheckValidationLayerSupport();
	static std::vector<const char*> getRequiredExtensions();
	
	static inline VkDebugUtilsMessengerEXT debugMessenger;
	static VkInstance instance;
};

