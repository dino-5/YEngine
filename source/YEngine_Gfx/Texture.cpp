#include <stdexcept>

#include "CommandBuffer.h"
#include "Texture.h"
#include "Device.h"
#include "GraphicsModule.h"
using namespace graphics;

void createImage(uint32_t width, uint32_t height, VkImage& image, VkDeviceMemory& memory, TextureCreateInfo textureInfo) {
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = textureInfo.format;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = textureInfo.usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.flags = 0; // Optional

	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkPhysicalDevice& physicalDevice = GraphicsModule::GetInstance()->getDevice().getPhysicalDevice().getDevice();

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		physicalDevice);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, memory, 0);
}

void Texture::release()
{
	VkDevice device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	vkDestroyImageView(device, m_view, nullptr);
	vkDestroyImage(device, m_image, nullptr);
	vkFreeMemory(device, m_imageMemory, nullptr);
	m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_isReleased = true;
}

void Texture::initAsTexture(const std::string& path, TextureCreateInfo textureInfo)
{
	m_isReleased = false;
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = static_cast<uint64_t>(texWidth) * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
	Buffer stagingBuffer;
	stagingBuffer.initAsStagingBuffer(imageSize, pixels);
	stbi_image_free(pixels);
	createImage( static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), m_image, m_imageMemory,
		textureInfo);

	CommandBuffer cmdBuffer = GraphicsModule::GetInstance()->getCommandPool().createOneTimeCmdBuffer();
	cmdBuffer.initAsSingleTimeCmdBuffer();
	getBarrierImageLayout(cmdBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureInfo.format);
	cmdBuffer.copyBufferToImage( stagingBuffer.getBuffer(), m_image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	getBarrierImageLayout(cmdBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureInfo.format);
	VkQueue graphicsQueue = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getGraphicsQueue();
	cmdBuffer.endSingleTimeCommands( graphicsQueue);
	createImageView(textureInfo);
	stagingBuffer.release();
}

void Texture::createImageView(TextureCreateInfo textureInfo)
{
	VkImageViewCreateInfo viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = m_image;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = textureInfo.format;
	viewCreateInfo.subresourceRange.aspectMask = textureInfo.aspectFlags;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	VkDevice device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	if (vkCreateImageView(device, &viewCreateInfo, nullptr, &m_view) != VK_SUCCESS)
		throw std::runtime_error("failed to create image view");
}

void Texture::initAsDepthBuffer(uint32_t width, uint32_t height, TextureCreateInfo textureInfo)
{
	m_isReleased = false;
	createImage(width, height, m_image, m_imageMemory, textureInfo);

	CommandBuffer cmdBuffer = GraphicsModule::GetInstance()->getCommandPool().createOneTimeCmdBuffer();
	cmdBuffer.initAsSingleTimeCmdBuffer();
	getBarrierImageLayout(cmdBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, textureInfo.format);

	createImageView(textureInfo);
	cmdBuffer.endSingleTimeCommands(GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getGraphicsQueue());
}

VkSampler createSampler()
{
	VkSampler sampler;
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkPhysicalDevice& physicalDevice = GraphicsModule::GetInstance()->getDevice().getPhysicalDevice().getDevice();

	VkPhysicalDeviceProperties deviceProperties = GetPhysicalDeviceProperties(physicalDevice);
	samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if(vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create image sampler");
	return sampler;

}

void Sampler::release()
{
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	vkDestroySampler(device, m_sampler, nullptr);
}

VkDescriptorImageInfo* Texture::getDescriptorImageInfo(Sampler sampler)
{
	info.imageLayout = m_currentLayout;
	info.imageView = m_view;
	info.sampler = sampler.getSampler();
	return &info;
}

bool hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Texture::getBarrierImageLayout(CommandBuffer& cmdBuffer, VkImageLayout newLayout, VkFormat format)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = m_currentLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;

	if (m_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (m_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (m_currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	cmdBuffer.pipelineBarrier(srcStage, dstStage, barrier);

	m_currentLayout = newLayout;
}
