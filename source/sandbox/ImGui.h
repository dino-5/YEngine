#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct ImGuiIO;

enum class ImGuiType
{
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
};

struct ImGuiEntry
{
	const char* name;
	void* address;
	ImGuiType type;
	float min;
	float max;
};

class ImGuiManager
{
public:
	static void Initialize();
	static void Release();
	static void StartFrame();
	static void Draw(VkCommandBuffer& cmdBuffer);

	static void AddImGuiEntry(ImGuiEntry entry);
private:
	static void drawInternal();
	static void displayEntry(ImGuiEntry entry);

	static inline ImGuiIO* io = nullptr;
	static inline std::vector<ImGuiEntry> s_entries;
};

