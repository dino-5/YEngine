#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "HelloTriangleApp.h"

#include "YEngine_System/system/Filesystem.h"
#include "YEngine_System/system/IOManager.h"

int main() {

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::cout << extensionCount << std::endl;

	HelloTriangleApplication app;
	bool test = 0;
	if (!test)
	{
		try {
			app.run();
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	if(test)
	{
		File file("shaders/shader.frag");
		io::print(file.readFile());
		Logger::Update();
	}

	return 0;

}