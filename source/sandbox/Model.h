#pragma once
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "tiny_obj_loader.h"
#include "graphics/Texture.h"
#include "Geometry.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Mesh
{
public:
	friend class std::shared_ptr<Mesh>;
	friend class std::unordered_map<std::string, std::shared_ptr<Mesh>>;
	static inline std::unordered_map<std::string, std::shared_ptr<Mesh>> s_meshes{};
	static std::shared_ptr<Mesh> CreateMesh(std::string a_name, const std::string& pathToModel)
;
public:
	std::vector<Geometry::Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	std::string name;
	void initBuffers() 
	{
		m_vertexBuffer.initAsVertexBuffer( m_vertices.size() * sizeof(Geometry::Vertex), m_vertices.data());
		m_indexBuffer.initAsIndexBuffer(m_indices.size() * sizeof(Geometry::IndexType), m_indices.data());
	}
	void release()
	{
		m_indexBuffer.release();
		m_vertexBuffer.release();
	}
	Mesh(std::string a_name) :name(a_name) {}
};

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

class Model
{
public:
	void init(const std::string& pathToPixels, TextureCreateInfo info, const std::string& pathToModel, 
		std::string name, std::string meshName);
	void update();
	void release()
	{
		for (uint32_t i = 0; i < 3; i++)
			m_texture[i].Release();
		if (m_mesh.use_count() == 2) // inside model and static map in Mesh
			m_mesh->release();
		else
			m_mesh.reset();
	}

	std::array<TextureImage, 3> m_texture;
	std::shared_ptr<Mesh> m_mesh;
	glm::vec3 m_position{};
	UniformBufferObject m_uniformObject;
	std::string m_name;
};

