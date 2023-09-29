#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <array>
#include <chrono>

#include "graphics/VulkanInstance.h"
#include "graphics/Device.h"
#include "graphics/Surface.h"
#include "graphics/SwapChain.h"
#include "graphics/GraphicsPipeline.h"
#include "graphics/CommandPool.h"
#include "graphics/CommandBuffer.h"
#include "graphics/Buffers.h"
#include "Geometry.h"
#include "graphics/DescriptorSet.h"
#include "graphics/Texture.h"
#include "graphics/GraphicsModule.h"

#include "Model.h"

#include "system/Window.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

	VkDescriptorSetLayoutBinding GetDescriptorSetBinding()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		return uboLayoutBinding;
	}
};

class HelloTriangleApplication 
{
public:
	static inline HelloTriangleApplication* m_instance = nullptr;
	HelloTriangleApplication()
	{
		if (m_instance == nullptr)
			m_instance = this;
		else
			throw std::runtime_error("tried to create one more app");
	}
	static const constexpr uint32_t WIDTH = 800;
	static const constexpr uint32_t HEIGHT = 600;
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void SetIsResized(bool fl)
	{
		m_graphicsModule->getSwapChain().setIsResized(fl);
	}

private:
	void initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window.init("Vulkan window", WIDTH, HEIGHT);
		m_window.setWindowResizeCallback(FrameBufferResizeCallback);
	}
	void initVulkan()
	{
		constexpr uint32_t numberOfPools = 2;
		VkDescriptorPoolSize poolSizes[numberOfPools] = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT} ,
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT*3 }
		};
		graphics::GraphicsModuleCreateInfo createInfo
		{
			.numberOfPools = numberOfPools,
			.poolSizes = poolSizes
		};
		m_graphicsModule = graphics::GraphicsModule::CreateGraphicsModule(createInfo);

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		constexpr uint32_t numberOfBindings= 2;
		VkDescriptorSetLayoutBinding descBindings[numberOfBindings] = { m_transform.GetDescriptorSetBinding(), samplerLayoutBinding };


		m_descriptorSetLayout.init(descBindings, numberOfBindings);
		m_pipeline.init( &m_descriptorSetLayout.getDescriptorSetLayout(), 1);
		m_descriptorSet.init( MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout.getDescriptorSetLayout());

		m_cmdBuffer.init( static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

		TextureCreateInfo textureInfo{
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
		};

		// TODO format added for image
		m_model.init("assets/diffuse.jpg", textureInfo, "assets/backpack.obj");

		m_vertices = m_model.m_vertices;
		m_vertexBuffer.initAsVertexBuffer( m_vertices.size() * sizeof(Geometry::Vertex), m_vertices.data());
		m_indices= m_model.m_indices;
		m_indexBuffer.initAsIndexBuffer(m_indices.size() * sizeof(Geometry::IndexType), m_indices.data());

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_uniformBuffer[i].init(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_uniformBuffer[i].mapMemory(0);

			constexpr uint32_t numberOfDescriptorSets = 2;
			VkWriteDescriptorSet descriptorSets[numberOfDescriptorSets];
			descriptorSets[0] = m_descriptorSet.getWriteDescriptor(i, m_uniformBuffer[i].getDescriptorBufferInfo(),
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			descriptorSets[1] = m_descriptorSet.getWriteDescriptor(i, m_model.m_texture[i].getDescriptorImageInfo(),
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_descriptorSet.updateDescriptors(descriptorSets, numberOfDescriptorSets);
		}
	}

	void mainLoop() 
	{
		while (!m_window.isWindowShouldBeClosed()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(m_graphicsModule->getDevice().getLogicalDevice().getDevice());
	}

	void updateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//m_transform.model = glm::mat4(1.0f);
		m_transform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_transform.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_transform.proj = glm::perspective(glm::radians(45.0f), m_graphicsModule->getSwapChain().getAspectRatio(), 0.1f, 10.0f);
		m_transform.proj[1][1] *= -1;

		m_uniformBuffer[m_currentFrame].copyMemory(&m_transform);
	}

	void drawFrame()
	{
		updateUniformBuffer();
		m_graphicsModule->waitForFences(m_currentFrame);
		int imageIndexT = m_graphicsModule->getCurrentFrameBufferIndex(m_currentFrame);
		if (imageIndexT == -1)
			return;
		uint32_t imageIndex = static_cast<uint32_t>(imageIndexT);
		m_graphicsModule->resetFences(m_currentFrame);
		m_cmdBuffer.reset(m_currentFrame);
		m_cmdBuffer.beginRenderPass(m_graphicsModule->getSwapChain().getRenderPassBeginInfo(imageIndex), m_currentFrame);
		m_cmdBuffer.bindGraphicsPipeline(m_pipeline.GetPipeline(), m_currentFrame);
		m_cmdBuffer.bindDescriptorSet(m_currentFrame, m_pipeline.GetPipelineLayout(),
			m_descriptorSet.getDescriptorSet(m_currentFrame));
		m_cmdBuffer.setViewport(SwapChain::getViewport(), m_currentFrame);
		m_cmdBuffer.setScissorRect(SwapChain::getScissorRect(), m_currentFrame);
		VkDeviceSize offsets[] = { 0 };
		m_cmdBuffer.bindVertexBuffers(m_currentFrame, &m_vertexBuffer.getBuffer(), offsets, 1);
		m_cmdBuffer.bindIndexBuffers(m_currentFrame, m_indexBuffer.getBuffer());
		m_cmdBuffer.drawIndexed(m_currentFrame, m_indices.size());
		m_cmdBuffer.endRenderPass(m_currentFrame);

		m_graphicsModule->submit(m_currentFrame, &m_cmdBuffer.getCmdBuffer(m_currentFrame));
		m_graphicsModule->present(m_currentFrame, imageIndex);

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void cleanup() {
		m_indexBuffer.release();
		m_vertexBuffer.release();
		m_model.release();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_uniformBuffer[i].release();
		}
		m_pipeline.Release();
		m_descriptorSetLayout.release();
		m_graphicsModule->release();
		m_window.release();
	}

private:
	Window m_window;
	graphics::GraphicsModule* m_graphicsModule;

	GraphicsPipeline m_pipeline;
	CommandBuffer m_cmdBuffer;
	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_uniformBuffer;
	UniformBufferObject m_transform;
	DescriptorSetLayout m_descriptorSetLayout;
	DescriptorSet m_descriptorSet;
	std::vector<Geometry::Vertex> m_vertices;
	std::vector<Geometry::IndexType> m_indices;
	uint32_t m_currentFrame = 0;
	Model m_model;
};

