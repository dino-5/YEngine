#pragma once

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
#include "YEngine_System/system/Logger.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void reloadShaderCallback();
VkDescriptorSetLayoutBinding getSamplerLayoutBinding();

constexpr uint32_t numberOfLayouts = 2;

struct InternalView
{
	struct Data
	{
		glm::mat4 view;
		glm::mat4 proj;
	};
	Data data;
	void init()
	{
		data.proj = glm::perspective(glm::radians(45.0f), 
			graphics::GraphicsModule::GetInstance()->getSwapChain().getAspectRatio(), 0.1f, 100.0f);
		data.proj[1][1] *= -1;
	}
	std::function<void()> update;
};

struct InternalLight 
{
	struct Data
	{
		glm::vec3 position{};
	};
	std::function<void()> update;
	Data data;
	void init() {}
};


class HelloTriangleApplication
{
public:
	static inline HelloTriangleApplication* m_instance = nullptr;
	HelloTriangleApplication():
		m_passBuffer(0, ShaderType::VERTEX),
		m_model(1, ShaderType::VERTEX),
		m_lightingModel(1, ShaderType::VERTEX),
		m_plane(1, ShaderType::VERTEX),
		m_lightBuffer(3, ShaderType::FRAGMENT)
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

	friend void reloadShaderCallback();

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
	void createPipelines()
	{
		constexpr uint32_t numberOfBindings = 4;
		VkDescriptorSetLayoutBinding descBindings[numberOfBindings] = {
			m_passBuffer.GetDescriptorSetBinding(), m_model.m_buffer.GetDescriptorSetBinding(),
			getSamplerLayoutBinding(), 	m_lightBuffer.GetDescriptorSetBinding()
		};
		{
			m_pipelines["lighting"] = GraphicsPipeline();
			m_descriptorSets["lightingPass"] = DescriptorSet();
			m_descriptorSets["lightGeometry"] = DescriptorSet();
			DescriptorSetLayoutCreateInfo dscSetLayoutCreateInfo = { descBindings, numberOfBindings };
			GraphicsPipelineCreateInfo pipelineCreateInfo
			{
				.vertexShader = "shaders/shader.vert",
				.fragmentShader = "shaders/shader.frag",
				.layoutCreateInfo = &dscSetLayoutCreateInfo,
				.layoutCount = 1 
			};
			m_pipelines["lighting"].init(pipelineCreateInfo);

			m_descriptorSets["lightingPass"].init(
				MAX_FRAMES_IN_FLIGHT, m_pipelines["lighting"].getDescriptorSetLayout(0)
			);
		}

		{
			m_pipelines["geometry"] = GraphicsPipeline();
			DescriptorSetLayoutCreateInfo dscSetLayoutCreateInfo = { descBindings, 3};
			GraphicsPipelineCreateInfo pipelineCreateInfo
			{
				.vertexShader = "shaders/shader.vert",
				.fragmentShader = "shaders/simpleShader.frag",
				.layoutCreateInfo = &dscSetLayoutCreateInfo,
				.layoutCount = 1
			};
			m_pipelines["geometry"].init(pipelineCreateInfo);
			m_descriptorSets["lightGeometry"].init(
				MAX_FRAMES_IN_FLIGHT, m_pipelines["geometry"].getDescriptorSetLayout(0)
			);
		}

		m_passBuffer.m_data.update = [&]()
		{
			m_passBuffer.m_data.data.view = m_camera.getView();
		};

		m_lightBuffer.m_data.update = [&]()
		{
			m_lightBuffer.m_data.data.position = m_lightingModel.m_position;
		};
	}
	void reloadPipelines()
	{
		for (auto& pipeline : m_pipelines)
			pipeline.second.reload();
	}
	void initVulkan()
	{
		m_camera.init(WIDTH / HEIGHT);
		constexpr uint32_t numberOfPools = 2;
		VkDescriptorPoolSize poolSizes[numberOfPools] = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT * 3} ,
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT }
		};
		graphics::GraphicsModuleCreateInfo createInfo
		{
			.numberOfPools = numberOfPools,
				.poolSizes = poolSizes
		};
		m_graphicsModule = graphics::GraphicsModule::CreateGraphicsModule(createInfo);
		ImGuiManager::Initialize();


		TextureCreateInfo textureInfo{
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
		};

		// TODO format added for image
		Mesh::CreateMesh("backpack", "assets/backpack.obj");
		Mesh::CreateMesh("plane", Geometry::GetPlaneVertices(), Geometry::GetPlaneIndices());
		m_model.init("assets/diffuse.jpg", textureInfo,  "model", "backpack");
		m_lightingModel.init("assets/diffuse.jpg", textureInfo, "lightingModel", "backpack");
		m_plane.init("assets/wall.jpg", textureInfo, "plane", "plane");
		m_passBuffer.init();
		m_lightBuffer.init();

		createPipelines();
		ImGuiManager::AddButton({ "shader Reload", &m_shaderReload});

		m_cmdBuffer.init(static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT));

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			{
				constexpr uint32_t numberOfDescriptors= 4;
				VkWriteDescriptorSet descriptors[numberOfDescriptors];
				auto& descriptorSet = m_descriptorSets["lightingPass"];

				descriptors[0] = descriptorSet.getWriteDescriptor(
					i, 0, m_passBuffer.m_buffers[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptors[1] = descriptorSet.getWriteDescriptor(
					i, 1, m_model.getDescriptorBufferInfo(i),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptors[2] = descriptorSet.getWriteDescriptor(
					i, 2, m_model.m_texture[i].getDescriptorImageInfo(),
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
				descriptors[3] = descriptorSet.getWriteDescriptor(
					i, 3, m_lightBuffer.m_buffers[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

				descriptorSet.updateDescriptors(descriptors, numberOfDescriptors);
			}

			{
				constexpr const uint32_t numberOfDescriptors = 3;
				VkWriteDescriptorSet descriptors[numberOfDescriptors];
				auto& descriptorSet = m_descriptorSets["lightGeometry"];

				descriptors[0] = descriptorSet.getWriteDescriptor(
					i, 0, m_passBuffer.m_buffers[i].getDescriptorBufferInfo(),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptors[1] = descriptorSet.getWriteDescriptor(
					i, 1, m_lightingModel.getDescriptorBufferInfo(i),
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
				descriptors[2] = descriptorSet.getWriteDescriptor(
					i, 2, m_lightingModel.m_texture[i].getDescriptorImageInfo(),
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

				descriptorSet.updateDescriptors(descriptors, numberOfDescriptors);
			}
		}
	}

	void mainLoop()
	{
		while (!m_window.isWindowShouldBeClosed()) {
			glfwPollEvents();
			drawFrame();
			Logger::Update();
			if (m_shaderReload)
			{
				for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
					m_graphicsModule->waitForFences(i);
				reloadPipelines();
				m_currentFrame = 0;
				m_shaderReload = false;
			}
		}
		vkDeviceWaitIdle(m_graphicsModule->getDevice().getLogicalDevice().getDevice());
	}

	void updateUniformBuffer()
	{
		m_model.update(m_currentFrame);
		m_lightingModel.update(m_currentFrame);
		m_passBuffer.update(m_currentFrame);
		m_lightBuffer.update(m_currentFrame);
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
			m_descriptorSets["lightingPass"].getDescriptorSet(m_currentFrame),
			m_descriptorSets["lightGeometry"].getDescriptorSet(m_currentFrame),
		};
		m_cmdBuffer.bindDescriptorSet(m_currentFrame, m_pipelines["lighting"].GetPipelineLayout(),
			descriptorSets, 1);
		m_cmdBuffer.setViewport(SwapChain::getViewport(), m_currentFrame);
		m_cmdBuffer.setScissorRect(SwapChain::getScissorRect(), m_currentFrame);
		VkDeviceSize offsets[] = { 0 };
		m_cmdBuffer.bindVertexBuffers(m_currentFrame, &m_model.m_mesh->m_vertexBuffer.getBuffer(), offsets, 1);
		m_cmdBuffer.bindIndexBuffers(m_currentFrame, m_model.m_mesh->m_indexBuffer.getBuffer());
		m_cmdBuffer.drawIndexed(m_currentFrame, m_model.m_mesh->m_indices.size());

		// TODO create descriptor set to contain different uniform object for matrices 
		m_cmdBuffer.bindGraphicsPipeline(m_pipelines["geometry"].GetPipeline(), m_currentFrame);
		m_cmdBuffer.bindDescriptorSet(m_currentFrame, m_pipelines["geometry"].GetPipelineLayout(),
			&descriptorSets[1], 1);
		m_cmdBuffer.drawIndexed(m_currentFrame, m_lightingModel.m_mesh->m_indices.size());

		// imgui render
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
		m_plane.release();
		m_lightBuffer.release();
		m_passBuffer.release();
		for (auto& pipeline : m_pipelines)
			pipeline.second.release();
		ImGuiManager::Release();
		Mesh::Cleanup();
		m_graphicsModule->release();
		m_window.release();
	}

private:
	Window m_window;
	graphics::GraphicsModule* m_graphicsModule;

	std::unordered_map<std::string, GraphicsPipeline> m_pipelines;
	std::unordered_map< std::string, DescriptorSet> m_descriptorSets;
	CommandBuffer m_cmdBuffer;
	UniformBuffer<InternalLight> m_lightBuffer;
	UniformBuffer<InternalView> m_passBuffer;
	Model m_model;
	Model m_lightingModel;
	Model m_plane;
	Camera m_camera;
	uint32_t m_currentFrame = 0;
	bool m_shaderReload = false;
};

