#pragma once
#include<vulkan/vulkan.h>

class VulkanInstance
{
public:
	static void CreateInstance();
	
	static VkInstance instance;
};

