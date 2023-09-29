#pragma once
#include "VulkanInstance.h"
#include "Device.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Surface.h"
#include "SwapChain.h"
#include "../common.h"

#include <memory>
namespace graphics
{

	struct GraphicsModuleCreateInfo
	{
		uint32_t numberOfPools;
		VkDescriptorPoolSize* poolSizes;
	};

#define geterGraphicsComponent(className, member) className& get##className() { return member; }

	class GraphicsModule
	{
	public:
		static GraphicsModule* GetInstance()
		{
			ASSERT(s_module != nullptr, "nullptr instance of graphics module");
			return s_module.get();
		}
		static GraphicsModule* CreateGraphicsModule(GraphicsModuleCreateInfo createInfo);
		static void ReleaseGraphicsModule();

		GraphicsModule(GraphicsModuleCreateInfo createInfo);

	private:
		static inline std::shared_ptr<GraphicsModule> s_module = nullptr;


	public:
		geterGraphicsComponent(Device, m_device)
		geterGraphicsComponent(CommandPool, m_cmdPool)
		geterGraphicsComponent(DescriptorPool, m_descriptorPool)
		geterGraphicsComponent(SwapChain, m_swapChain)

		void createSyncObjects();
		void swapChainInit();
		void release();

		void resetFences(uint32_t frame);
		void waitForFences(uint32_t frame);
		int getCurrentFrameBufferIndex(uint32_t frame);
		void submit(uint32_t frame, const VkCommandBuffer* commandBuffer);
		void present(uint32_t frame, uint32_t imageIndex);

	private:
		Surface m_surface;
		Device m_device;
		CommandPool m_cmdPool;
		SwapChain m_swapChain;
		DescriptorPool m_descriptorPool;

		std::vector<VkSemaphore> m_imageAvailableSemph;
		std::vector<VkSemaphore> m_renderFinishedSemph;
		std::vector<VkFence> m_inFlightFence;
	};
};