#include "HelloTriangleApp.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	HelloTriangleApplication::m_instance->SetIsResized(true);
}
