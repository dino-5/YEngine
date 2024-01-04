#include "RenderPass.h"
#include <array>
#include <stdexcept>

std::vector<VkAttachmentDescription> getColorAttachments(PassInfo info)
{

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = info.imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	return std::vector<VkAttachmentDescription>(info.numberOfColorAttachments, colorAttachment);
}

VkAttachmentDescription getDepthAttachment()
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	return depthAttachment;
}

VkAttachmentReference createColorAttachmentRef(uint32_t attachment)
{
	return VkAttachmentReference{
		.attachment = attachment,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
} 

VkAttachmentReference createDepthAttachmentRef(uint32_t attachment)
{
	return VkAttachmentReference{
		.attachment = attachment,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
} 

void RenderPass::init(VkDevice& device, PassInfo info)
{
	m_device = &device;

	std::vector<VkAttachmentDescription> attachments = getColorAttachments(info);

	std::vector<VkAttachmentReference> colorAttachmentRef;
	for (int i = 0; i < attachments.size(); i++)
		colorAttachmentRef.push_back(createColorAttachmentRef(i));

	VkAttachmentReference depthAttachmentRef;
	if (info.numberOfDepthAttachments)
	{
		attachments.push_back(getDepthAttachment());
		depthAttachmentRef = createDepthAttachmentRef(attachments.size()-1);
	}

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = colorAttachmentRef.size();
	subpass.pColorAttachments = colorAttachmentRef.data();
	subpass.pDepthStencilAttachment = info.numberOfDepthAttachments ? &depthAttachmentRef : nullptr;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;


	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}

}
