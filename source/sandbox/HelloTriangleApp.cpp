#include "HelloTriangleApp.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	HelloTriangleApplication::m_instance->SetIsResized(true);
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	HelloTriangleApplication::m_instance->cursorPositionCallback(window, xpos, ypos);
}

void HelloTriangleApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	
}

void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	HelloTriangleApplication::m_instance->keyInputCallback(window, key, scancode, action, mods);
}

void HelloTriangleApplication::keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	m_camera.parseKeyInput(window, key, scancode, action, mods);
}

