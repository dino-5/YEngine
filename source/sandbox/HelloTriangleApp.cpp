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
	m_camera.parseMouseInput(window, xpos, ypos);
}

void reloadShaderCallback()
{
	HelloTriangleApplication::m_instance->reloadPipelines();
}

void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	HelloTriangleApplication::m_instance->keyInputCallback(window, key, scancode, action, mods);
}

void HelloTriangleApplication::keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	m_camera.parseKeyInput(window, key, scancode, action, mods);
}

VkDescriptorSetLayoutBinding getSamplerLayoutBinding()
{
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	return samplerLayoutBinding;
 }
