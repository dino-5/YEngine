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
	BUTTON,
};

struct ImGuiEntry
{
	const char* name;
	void* address;
	ImGuiType type;
	float min;
	float max;
};

typedef void (*FunctionPtr)();
struct ImGuiCallback
{
	const char* name;
	bool* fl;
};

class ImGuiManager
{
public:
	static void Initialize();
	static void Release();
	static void StartFrame();
	static void Draw(VkCommandBuffer& cmdBuffer);
	static void AddButton(ImGuiCallback callback);

	static void AddImGuiEntry(ImGuiEntry entry);
private:
	static void drawInternal();
	static void displayEntry(ImGuiEntry entry);

	static inline ImGuiIO* io = nullptr;
	static inline std::vector<ImGuiEntry> s_entries;
	static inline std::vector<ImGuiCallback> s_buttons;
};

