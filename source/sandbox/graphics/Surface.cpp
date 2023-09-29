#include "Surface.h"
#include <stdexcept>
#include "VulkanInstance.h"
#define VK_USE_PlATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_win32.h>
#include "../system/Window.h"

void Surface::init()
{
	Window* window = Window::GetWindow();
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = window->GetWindowHandle();
	surfaceCreateInfo.hinstance = window->GetInstance();

	if (vkCreateWin32SurfaceKHR(VulkanInstance::GetInstance(), &surfaceCreateInfo, nullptr, &m_surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create surface");
}

void Surface::release()
{
	vkDestroySurfaceKHR(VulkanInstance::GetInstance(), m_surface, nullptr);
}
