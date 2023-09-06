#include "Window.h"

void Window::init(const char* name, uint width, uint height)
{
	m_width = width;
	m_height = height;
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
}

void Window::release()
{
	glfwDestroyWindow(m_window); 
	glfwTerminate();
}

void Window::setWindowResizeCallback(GLFWframebuffersizefun ptr)
{
	glfwSetFramebufferSizeCallback(m_window, ptr);
}

HWND Window::GetWindowHandle()
{
	return glfwGetWin32Window(m_window);
}

HINSTANCE Window::GetInstance()
{
	return GetModuleHandle(nullptr);
}

void Window::update()
{

}
