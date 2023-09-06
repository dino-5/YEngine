#pragma once
#include <vulkan/vulkan.h>

class Framebuffer
{
public:
	void Init();
private:
	VkFramebuffer m_frameBuffer;
};

