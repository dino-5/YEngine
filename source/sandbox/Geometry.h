#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>

namespace Geometry
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}
		using VertexAttributeDescArray = std::array<VkVertexInputAttributeDescription, 2>;
		static VertexAttributeDescArray GetAttributeDescriptions()
		{
			VertexAttributeDescArray attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);
			return attributeDescriptions;
		}

	};
	using IndexType = uint32_t;
	const std::vector<Vertex> GetDefaultVertices();
	const std::vector<IndexType> GetDefaultIndices();
};

