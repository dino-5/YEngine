#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

class ImGui
{
public:
	static ImGui* GetInstance()
	{
		return s_instance;
	}
	static void Initialize();
private:
	static inline ImGui* s_instance = nullptr;
};

