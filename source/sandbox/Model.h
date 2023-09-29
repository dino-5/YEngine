#pragma once
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <string>

#include "tiny_obj_loader.h"
#include "graphics/Texture.h"
#include "Geometry.h"

class Model
{
public:
	void init(const std::string& pathToPixels, TextureCreateInfo info, const std::string& pathToModel);
	void release()
	{
		for (uint32_t i = 0; i < 3; i++)
			m_texture[i].Release();
	}

	std::array<TextureImage, 3> m_texture;
	std::vector<Geometry::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
};

