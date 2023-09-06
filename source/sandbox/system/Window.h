#pragma once
#include "../common.h"
#define VK_USE_PlATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_win32.h>


class Window
{
private:
	static inline Window* s_window = nullptr;
public:
	static Window* GetWindow() {
		return s_window;
	}
	NON_COPYABLE(Window);
	Window()
	{
		if (s_window == nullptr)
			s_window = this;
		else
			throw std::runtime_error("trying to create one more Window");
	}
	~Window()
	{
		s_window = nullptr;
	}
	void init(const char* name, uint width, uint height);
	void setWindowResizeCallback(GLFWframebuffersizefun ptr);
	void release();
	void update();
	void GetFrameBufferSize(int& width, int& height) { glfwGetFramebufferSize(m_window, &width, &height); }
	bool isWindowShouldBeClosed() { return glfwWindowShouldClose(m_window); }
	HWND GetWindowHandle();
	HINSTANCE GetInstance();
private:
	GLFWwindow* m_window = nullptr;
	uint m_width = 0;
	uint m_height = 0;
};

