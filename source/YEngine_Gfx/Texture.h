#pragma once
#include <stb_image.h>

#include <vulkan/vulkan.h>
#include <string>

#include "Buffers.h"

class CommandBuffer;

VkSampler createSampler();
class Sampler
{
public:
	void init()
	{
		m_sampler = createSampler();
	}
	void release();
	VkSampler getSampler() { return m_sampler; }
private:
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
	void initAsTexture(	const std::string& path, TextureCreateInfo textureInfo);
	void initAsDepthBuffer(uint32_t width, uint32_t height, TextureCreateInfo textureInfo);

	void getBarrierImageLayout(CommandBuffer& cmdBuffer, VkImageLayout newLayout, VkFormat format);
	VkImageView getImageView() { return m_view; }
	VkDescriptorImageInfo* getDescriptorImageInfo(Sampler sampler);

	void release();
	void createImageView(TextureCreateInfo textureInfo);
	~Texture()
	{
		if (!m_isReleased)
			throw std::runtime_error("Texture is not destroyed");
	}

private:
	bool m_isReleased = true;
	VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImage m_image;
	VkImageView m_view;
	VkDeviceMemory m_imageMemory;
	VkDescriptorImageInfo info;
};

struct TextureImage
{
	Texture texture;
	Sampler sampler;
	void initAsTexture(const std::string& path, TextureCreateInfo info)
	{
		texture.initAsTexture(path, info);
		sampler.init();
	}

	void initAsDepthBuffer(uint32_t width, uint32_t height, TextureCreateInfo textureInfo)
	{
		texture.initAsDepthBuffer(width, height, textureInfo);
		sampler.init();
	}
	void release()
	{
		texture.release();
		sampler.release();
	}
	VkDescriptorImageInfo* getDescriptorImageInfo() {
		return texture.getDescriptorImageInfo(sampler);
	}
};

