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
#include <unordered_map>

#include "YEngine_Gfx/VulkanInstance.h"
#include "YEngine_Gfx/Device.h"
#include "YEngine_Gfx/Surface.h"
#include "YEngine_Gfx/SwapChain.h"
#include "YEngine_Gfx/GraphicsPipeline.h"
#include "YEngine_Gfx/CommandPool.h"
#include "YEngine_Gfx/CommandBuffer.h"
#include "YEngine_Gfx/Buffers.h"
#include "YEngine_Gfx/DescriptorSet.h"
#include "YEngine_Gfx/Texture.h"
#include "YEngine_Gfx/Model.h"
#include "YEngine_Gfx/GraphicsModule.h"
#include "YEngine_Gfx/ImGui.h"

#include "YEngine_System/Geometry.h"
#include "YEngine_System/Camera.h"
#include "YEngine_System/system/Window.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


struct UniformLightingStruct
{
	glm::vec3 position{};

	void imGuiInit()
	{
		ImGuiEntry entry;
		entry.name = "lighting position";
		entry.address = &position[0];
		entry.min = -10.f;
		entry.max =  10.f;
		entry.type = ImGuiType::FLOAT3;
		ImGuiManager::AddImGuiEntry(entry);
	}

	VkDescriptorSetLayoutBinding GetDescriptorSetBinding()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		return uboLayoutBinding;
	}
};

struct FrameResources
{
};

struct PassInfo
{
	std::string name;
	std::string vertexShader;
	std::string fragmentShader;

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

	void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
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
		m_window.setCursorPositionCallback(::cursorPositionCallback);
		m_window.setKeyInputCallback(::keyInputCallback);
	}
	void initVulkan()
	{
		m_camera.init(WIDTH / HEIGHT);
		constexpr uint32_t numberOfPools = 2;
		VkDescriptorPoolSize poolSizes[numberOfPools] = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT*3} ,
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT }
		};
		graphics::GraphicsModuleCreateInfo createInfo
		{
			.numberOfPools = numberOfPools,
			.poolSizes = poolSizes
		};
		m_graphicsModule = graphics::GraphicsModule::CreateGraphicsModule(createInfo);
		ImGuiManager::Initialize();

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		TextureCreateInfo textureInfo{
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
		};

		// TODO format added for image
		m_model.init("assets/diffuse.jpg", textureInfo, "assets/backpack.obj", "model", "backpack");
		m_lightingModel.init("assets/diffuse.jpg", textureInfo, "assets/backpack.obj", "lightingModel", "backpack");
		m_lighting.imGuiInit();

		constexpr uint32_t numberOfBindings = 3;
		VkDescriptorSetLayoutBinding descBindings[numberOfBindings] = {
			m_model.m_uniformObject.GetDescriptorSetBinding(), samplerLayoutBinding,
			m_lighting.GetDescriptorSetBinding()
		};

		constexpr uint32_t numberOfLayouts = 2;
		DescriptorSetLayoutCreateInfo layoutInfo[numberOfLayouts] =
		{
			{descBindings, 2},
			{&descBindings[2], 1}
		};
		{
			m_pipelines["lighting"] = GraphicsPipeline();
			m_descriptorSets["lighting"] = DescriptorSet();
			m_descriptorSets["geometry"] = DescriptorSet();
			GraphicsPipelineCreateInfo pipelineCreateInfo
			{
				.vertexShader = "shaders/bin/vert.spv" ,
				.fragmentShader = "shaders/bin/frag.spv",
				.layoutCreateInfo = layoutInfo,
				.layoutCount = numberOfLayouts 
			};
			m_pipelines["lighting"].init(pipelineCreateInfo);

			m_descriptorSets["geometry1"].init(
				MAX_FRAMES_IN_FLIGHT, m_pipelines["lighting"].getDescriptorSetLayout(0)
			);
			m_descriptorSets["geometry2"].init(
				MAX_FRAMES_IN_FLIGHT, m_pipelines["lighting"].getDescriptorSetLayout(0)
			);
			m_descriptorSets["lighting"].init(
				MAX_FRAMES_IN_FLIGHT, m_pipelines["lighting"].getDescriptorSetLayout(1)
			);
		}

		{
			m_pipelines["geometry"] = GraphicsPipeline();
			GraphicsPipelineCreateInfo pipelineCreateInfo
			{
				.vertexShader = "shaders/bin/vert.spv" ,
				.fragmentShader = "shaders/bin/simpleFrag.spv",
				.layoutCreateInfo = layoutInfo,
				.layoutCount = 1
			};
			m_pipelines["geometry"].init(pipelineCreateInfo);
		}

		m_cmdBuffer.init( static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));


		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_uniformBuffer[i].init(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_uniformBuffer[i].mapMemory(0);

			m_lightingModelBuffer[i].init(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_lightingModelBuffer[i].mapMemory(0);

			m_lightingBuffer[i].init(sizeof(UniformLightingStruct), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			m_lightingBuffer[i].mapMemory(0);
			{
				constexpr uint32_t numberOfDescriptorSets = 3;
				VkWriteDescriptorSet descriptorSets[numberOfDescriptorSets];
				descriptorSets[0] = m_descriptorSets["geometry1"].getWriteDescriptor(
					i, 0, m_uniformBuffer[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptorSets[1] = m_descriptorSets["geometry1"].getWriteDescriptor(
					i, m_model.m_texture[i].getDescriptorImageInfo(),
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
				descriptorSets[2] = m_descriptorSets["lighting"].getWriteDescriptor(
					i, 0, m_lightingBuffer[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

				m_descriptorSets["geometry1"].updateDescriptors(
					descriptorSets, 2);

				m_descriptorSets["lighting"].updateDescriptors(
					&descriptorSets[2], 1);
			}
			
			{
				VkWriteDescriptorSet descriptorSets[2];
				descriptorSets[0] = m_descriptorSets["geometry2"].getWriteDescriptor(
					i, 0, m_lightingModelBuffer[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptorSets[1] = m_descriptorSets["geometry2"].getWriteDescriptor(
					i, m_lightingModel.m_texture[i].getDescriptorImageInfo(),
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
				m_descriptorSets["geometry2"].updateDescriptors(
					descriptorSets, 2);
			}
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
		m_model.update();
		m_lightingModel.update();

		m_model.m_uniformObject.view = m_camera.getView();
		m_lightingModel.m_uniformObject.view = m_camera.getView();
		m_uniformBuffer[m_currentFrame].copyMemory(&m_model.m_uniformObject);
		m_lightingBuffer[m_currentFrame].copyMemory(&m_lightingModel.m_position);
		m_lightingModelBuffer[m_currentFrame].copyMemory(&m_lightingModel.m_uniformObject);
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
		m_cmdBuffer.bindGraphicsPipeline(m_pipelines["lighting"].GetPipeline(), m_currentFrame);
		VkDescriptorSet descriptorSets[] = {
			m_descriptorSets["geometry1"].getDescriptorSet(m_currentFrame),
			m_descriptorSets["lighting"].getDescriptorSet(m_currentFrame),
			m_descriptorSets["geometry2"].getDescriptorSet(m_currentFrame),
		};
		m_cmdBuffer.bindDescriptorSet(m_currentFrame, m_pipelines["lighting"].GetPipelineLayout(),
		         descriptorSets, 2);
		m_cmdBuffer.setViewport(SwapChain::getViewport(), m_currentFrame);
		m_cmdBuffer.setScissorRect(SwapChain::getScissorRect(), m_currentFrame);
		VkDeviceSize offsets[] = { 0 };
		m_cmdBuffer.bindVertexBuffers(m_currentFrame, &m_model.m_mesh->m_vertexBuffer.getBuffer(), offsets, 1);
		m_cmdBuffer.bindIndexBuffers(m_currentFrame, m_model.m_mesh->m_indexBuffer.getBuffer());
		m_cmdBuffer.drawIndexed(m_currentFrame, m_model.m_mesh->m_indices.size());

		// TODO create descriptor set to contain different uniform object for matrices 
		m_cmdBuffer.bindGraphicsPipeline(m_pipelines["geometry"].GetPipeline(), m_currentFrame);
		m_cmdBuffer.bindDescriptorSet(m_currentFrame, m_pipelines["lighting"].GetPipelineLayout(),
		         &descriptorSets[2], 1);
		m_cmdBuffer.drawIndexed(m_currentFrame, m_lightingModel.m_mesh->m_indices.size());
		ImGuiManager::StartFrame();
		ImGuiManager::Draw(m_cmdBuffer.getCmdBuffer(m_currentFrame));
		m_cmdBuffer.endRenderPass(m_currentFrame);

		m_graphicsModule->submit(m_currentFrame, &m_cmdBuffer.getCmdBuffer(m_currentFrame));
		m_graphicsModule->present(m_currentFrame, imageIndex);

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void cleanup() {
		m_model.release();
		m_lightingModel.release();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_uniformBuffer[i].release();
			m_lightingBuffer[i].release();
			m_lightingModelBuffer[i].release();
		}
		for (auto& pipeline : m_pipelines)
			pipeline.second.release();
		ImGuiManager::Release();
		m_graphicsModule->release();
		m_window.release();
	}

private:
	Window m_window;
	graphics::GraphicsModule* m_graphicsModule;

	std::unordered_map<std::string, GraphicsPipeline> m_pipelines;
	CommandBuffer m_cmdBuffer;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_uniformBuffer;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_lightingBuffer;
	std::array<Buffer, MAX_FRAMES_IN_FLIGHT> m_lightingModelBuffer;
	UniformLightingStruct m_lighting;
	std::unordered_map< std::string, DescriptorSet> m_descriptorSets;
	uint32_t m_currentFrame = 0;
	Model m_model;
	Model m_lightingModel;
	Camera m_camera;
};

