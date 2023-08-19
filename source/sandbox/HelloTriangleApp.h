#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include"VulkanInstance.h"

class HelloTriangleApplication {
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
	}

	void mainLoop() 
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {

		VulkanInstance::DestroyVulkan();
		glfwDestroyWindow(window); 
		glfwTerminate();
	}

private:
	GLFWwindow* window;
};

