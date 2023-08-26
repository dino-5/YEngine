#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "VulkanInstance.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"

class HelloTriangleApplication 
{
public:
	static const constexpr uint32_t WIDTH = 800;
	static const constexpr uint32_t HEIGHT = 600;
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initWindow()
	{

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
	}
	void initVulkan() {
		VulkanInstance::InitVulkan();
		m_surface.Init(window);
		m_physicalDevice.Init(m_surface.GetSurface());
		m_logicalDevice.Init(m_physicalDevice.GetDevice(), m_surface.GetSurface());
		m_swapChain.Init(window, m_physicalDevice.GetDevice(), m_logicalDevice.GetDevice(), m_surface.GetSurface());
		m_pipeline.Init(m_logicalDevice.GetDevice(), m_swapChain.GetExtent(), m_swapChain.GetFormat());
	}

	void mainLoop() 
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		m_pipeline.Release();
		m_swapChain.Release();
		m_surface.Release();
		m_logicalDevice.Release();
		VulkanInstance::DestroyVulkan();
		glfwDestroyWindow(window); 
		glfwTerminate();
	}

private:
	GLFWwindow* window;
	PhysicalDevice m_physicalDevice;
	LogicalDevice m_logicalDevice;
	Surface m_surface;
	SwapChain m_swapChain;
	GraphicsPipeline m_pipeline;
};

