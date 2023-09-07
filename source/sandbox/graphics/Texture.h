#pragma once
#include <stb_image.h>

#include <vulkan/vulkan.h>
#include <string>

#include "Buffers.h"

class CommandBuffer;

VkSampler CreateSampler(VkDevice& device, VkPhysicalDevice physicaldevice);
class Sampler
{
public:
	void Init(VkDevice& device, VkPhysicalDevice physicaldevice)
	{
		m_device = &device;
		m_sampler = CreateSampler(device, physicaldevice);
	}
	void Release()
	{
		vkDestroySampler(*m_device, m_sampler, nullptr);
	}
	VkSampler& GetSampler() { return m_sampler; }
private:
	VkDevice* m_device;
	VkSampler m_sampler;
};

struct TextureCreateInfo
{
	VkFormat format{};
	VkImageUsageFlags usage{};
	VkImageAspectFlags aspectFlags{};
};

class Texture
{
public:
	void InitAsTexture(VkDevice& device, VkPhysicalDevice& physicaldevice, VkCommandPool& cmdPool, VkQueue& queue,
		const std::string& path, TextureCreateInfo textureInfo);
	void InitAsDepthBuffer(VkDevice& device, VkPhysicalDevice& physicaldevice, VkCommandPool& cmdPool, VkQueue& queue,
		uint32_t width, uint32_t height, TextureCreateInfo textureInfo);
	void GetBarrierImageLayout(CommandBuffer& cmdBuffer, VkImageLayout newLayout, VkFormat format);

	VkImageView GetImageView() { return m_view; }

	void Release()
	{
		vkDestroyImageView(*m_device, m_view, nullptr);
		vkDestroyImage(*m_device, m_image, nullptr);
		vkFreeMemory(*m_device, m_imageMemory, nullptr);
	}
	VkDescriptorImageInfo GetDescriptorImageInfo(Sampler sampler);
	void CreateImageView(TextureCreateInfo textureInfo);
private:
	VkDevice* m_device;
	VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImage m_image;
	VkImageView m_view;
	VkDeviceMemory m_imageMemory;
};

struct TextureImage
{
	Texture texture;
	Sampler sampler;
	void Init(VkDevice& device, VkPhysicalDevice& physicaldevice, VkCommandPool& cmdPool, VkQueue& queue,
		const std::string& path, TextureCreateInfo info)
	{
		texture.InitAsTexture(device, physicaldevice, cmdPool, queue, path, info);
		sampler.Init(device, physicaldevice);
	}
	void Release()
	{
		texture.Release();
		sampler.Release();
	}
	VkDescriptorImageInfo GetDescriptorImageInfo() {
		return texture.GetDescriptorImageInfo(sampler);
	}
};

