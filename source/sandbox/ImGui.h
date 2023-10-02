#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct ImGuiIO;

class ImGuiManager
{
public:
	static void Initialize();
	static void Release();
	static void StartFrame();
	static void Draw(VkCommandBuffer& cmdBuffer);
private:
	static void drawInternal();

	static inline ImGuiIO* io = nullptr;
	//static std::vector<>;
};

