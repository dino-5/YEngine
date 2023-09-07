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
		m_swapChain.SetIsResized(fl);
	}

private:
	void initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window.init("Vulkan window", WIDTH, HEIGHT);
		m_window.setWindowResizeCallback(FrameBufferResizeCallback);
	}
	void initVulkan() {
		VulkanInstance::InitVulkan();
		m_surface.Init();
		m_physicalDevice.Init(m_surface.GetSurface());
		m_logicalDevice.Init(m_physicalDevice.GetDevice(), m_surface.GetSurface());
		m_pool.Init(m_physicalDevice.GetDevice(), m_surface.GetSurface(), m_logicalDevice.GetDevice());
		m_swapChain.Init(m_physicalDevice.GetDevice(), m_logicalDevice.GetDevice(), m_surface.GetSurface(), 
			m_logicalDevice.GetGraphicsQueue(), m_pool.GetPool());

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		constexpr uint32_t numberOfBindings= 2;
		VkDescriptorSetLayoutBinding descBindings[numberOfBindings] = { m_transform.GetDescriptorSetBinding(), samplerLayoutBinding };
		m_descriptorSetLayout.Init(m_logicalDevice.GetDevice(), descBindings, numberOfBindings);
		m_pipeline.Init(m_logicalDevice.GetDevice(), m_swapChain, &m_descriptorSetLayout.GetDescriptorSetLayout(), 1);

		m_cmdBuffer.Init(m_logicalDevice.GetDevice(), m_pool.GetPool(), static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));
		createSyncObjects();

		constexpr uint32_t numberOfPools = 2;
		VkDescriptorPoolSize poolSizes[numberOfPools] = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT} ,
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT*3 }
		};
		m_descriptorPool.Init(m_logicalDevice.GetDevice(), poolSizes, numberOfPools);
		m_descriptorSet.Init(m_logicalDevice.GetDevice(), MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout.GetDescriptorSetLayout(),
			m_descriptorPool.GetDescriptorPool());

		TextureCreateInfo textureInfo{
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
		};
		// TODO format added for image
		m_model.Init(m_logicalDevice.GetDevice(), m_physicalDevice.GetDevice(), m_pool.GetPool(),
			m_logicalDevice.GetGraphicsQueue(), "assets/diffuse.jpg", textureInfo,
			"assets/backpack.obj");

		m_vertices = m_model.m_vertices;
		m_vertexBuffer.InitAsVertexBuffer(m_logicalDevice.GetDevice(), m_physicalDevice.GetDevice(), m_pool.GetPool(),
			m_logicalDevice.GetGraphicsQueue(), m_vertices.size() * sizeof(Geometry::Vertex), m_vertices.data());
		m_indices= m_model.m_indices;
		m_indexBuffer.InitAsIndexBuffer(m_logicalDevice.GetDevice(), m_physicalDevice.GetDevice(), m_pool.GetPool(),
			m_logicalDevice.GetGraphicsQueue(),m_indices.size() * sizeof(Geometry::IndexType), m_indices.data());



		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			// creating of texture
			//m_texture[i].Init(m_logicalDevice.GetDevice(), m_physicalDevice.GetDevice(), m_pool.GetPool(),
			//	m_logicalDevice.GetGraphicsQueue(), "assets/texture.jpg", textureInfo);


			m_uniformBuffer[i].Init(m_logicalDevice.GetDevice(), m_physicalDevice.GetDevice(), sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_uniformBuffer[i].MapMemory(0);

			constexpr uint32_t numberOfDescriptorSets = 2;
			VkWriteDescriptorSet descriptorSets[numberOfDescriptorSets];
			descriptorSets[0] = m_descriptorSet.GetWriteDescriptor(i, m_uniformBuffer[i].GetDescriptorBufferInfo(),
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			descriptorSets[1] = m_descriptorSet.GetWriteDescriptor(i, m_model.m_texture[i].GetDescriptorImageInfo(),
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			m_descriptorSet.UpdateDescriptors(descriptorSets, numberOfDescriptorSets);
		}

	}

	void createSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		m_imageAvailableSemph.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemph.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			if (vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemph[i]) ||
				vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemph[i]) ||
				vkCreateFence(m_logicalDevice.GetDevice(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create syncronising objects");
		}
	}

	void mainLoop() 
	{
		while (!m_window.isWindowShouldBeClosed()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(m_logicalDevice.GetDevice());
	}

	void updateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//m_transform.model = glm::mat4(1.0f);
		m_transform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_transform.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_transform.proj = glm::perspective(glm::radians(45.0f), m_swapChain.GetAspectRatio(), 0.1f, 10.0f);
		m_transform.proj[1][1] *= -1;
#undef CopyMemory
		m_uniformBuffer[m_currentFrame].CopyMemory(&m_transform);
	}

	void drawFrame()
	{
		updateUniformBuffer();
		vkWaitForFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);
		int imageIndexT = m_swapChain.GetCurrentFrameBufferIndex(m_imageAvailableSemph[m_currentFrame], 
			m_logicalDevice.GetGraphicsQueue(), m_pool.GetPool());
		if (imageIndexT == -1)
			return;
		uint32_t imageIndex = static_cast<uint32_t>(imageIndexT);
		vkResetFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFence[m_currentFrame]);
		m_cmdBuffer.Reset(m_currentFrame);
		m_cmdBuffer.BeginRenderPass(m_swapChain.GetRenderPassBeginInfo(imageIndex), m_currentFrame);
		m_cmdBuffer.BindGraphicsPipeline(m_pipeline.GetPipeline(), m_currentFrame);
		m_cmdBuffer.BindDescriptorSet(m_currentFrame, m_pipeline.GetPipelineLayout(), m_descriptorSet.GetDescriptorSet(m_currentFrame));
		m_cmdBuffer.SetViewport(SwapChain::GetViewport(), m_currentFrame);
		m_cmdBuffer.SetScissorRect(SwapChain::GetScissorRect(), m_currentFrame);
		VkDeviceSize offsets[] = { 0 };
		m_cmdBuffer.BindVertexBuffers(m_currentFrame, &m_vertexBuffer.GetBuffer(), offsets, 1);
		m_cmdBuffer.BindIndexBuffers(m_currentFrame, m_indexBuffer.GetBuffer());
		m_cmdBuffer.DrawIndexed(m_currentFrame, m_indices.size());
		m_cmdBuffer.EndRenderPass(m_currentFrame);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemph[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_cmdBuffer.GetCmdBuffer(m_currentFrame);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_renderFinishedSemph[m_currentFrame];
		if (vkQueueSubmit(m_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_renderFinishedSemph[m_currentFrame];
		VkSwapchainKHR swapChains[] = { m_swapChain.GetSwapChain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		vkQueuePresentKHR(m_logicalDevice.GetPresentationQueue(), &presentInfo);
		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void cleanup() {
		m_indexBuffer.Release();
		m_vertexBuffer.Release();
		m_model.Release();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			//m_texture[i].Release();
			m_uniformBuffer[i].Release();
			vkDestroySemaphore(m_logicalDevice.GetDevice(), m_renderFinishedSemph[i], nullptr);
			vkDestroySemaphore(m_logicalDevice.GetDevice(), m_imageAvailableSemph[i], nullptr);
			vkDestroyFence(m_logicalDevice.GetDevice(), m_inFlightFence[i], nullptr);
		}
		m_pool.Release();
		m_descriptorPool.Release();
		m_pipeline.Release();
		m_descriptorSetLayout.Release();
		m_swapChain.Release();
		m_surface.Release();
		m_logicalDevice.Release();
		VulkanInstance::DestroyVulkan();
		m_window.release();
	}


private:
	Window m_window;
	PhysicalDevice m_physicalDevice;
	LogicalDevice m_logicalDevice;
	Surface m_surface;
	SwapChain m_swapChain;
	GraphicsPipeline m_pipeline;
	CommandPool m_pool;
	CommandBuffer m_cmdBuffer;
	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_uniformBuffer;
	std::array<Texture, MAX_FRAMES_IN_FLIGHT> m_depthTexture;
	UniformBufferObject m_transform;
	DescriptorSetLayout m_descriptorSetLayout;
	DescriptorPool m_descriptorPool;
	DescriptorSet m_descriptorSet;
	std::array<TextureImage, MAX_FRAMES_IN_FLIGHT> m_texture;
	std::vector<Geometry::Vertex> m_vertices;
	std::vector<Geometry::IndexType> m_indices;
	std::vector<VkSemaphore> m_imageAvailableSemph;
	std::vector<VkSemaphore> m_renderFinishedSemph;
	std::vector<VkFence> m_inFlightFence;
	uint32_t m_currentFrame = 0;
	Model m_model;
};

