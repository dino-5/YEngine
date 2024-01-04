#pragma once
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "tiny_obj_loader.h"
#include "Texture.h"
#include "gfx_common.h"
#include "YEngine_System/Geometry.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Mesh
{
public:
	using Vertex = Geometry::Vertex;
	using IndexType = Geometry::IndexType;
	friend class std::shared_ptr<Mesh>;
	friend class std::unordered_map<std::string, std::shared_ptr<Mesh>>;
	static inline std::unordered_map<std::string, std::shared_ptr<Mesh>> s_meshes{};
	static std::shared_ptr<Mesh> GetMesh(std::string a_name);
	static std::shared_ptr<Mesh> CreateMesh(std::string a_name, const std::string& pathToModel);
	static std::shared_ptr<Mesh> CreateMesh(std::string a_name,
			const std::vector<Vertex> vertices,
			const std::vector<IndexType> indices);
	static void DeleteMesh(std::string name);
	static void Cleanup();
;
public:
	std::vector<Vertex> m_vertices;
	std::vector<IndexType> m_indices;
	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	std::string name;
	void initBuffers() 
	{
		m_vertexBuffer.initAsVertexBuffer( m_vertices.size() * sizeof(Vertex), m_vertices.data());
		m_indexBuffer.initAsIndexBuffer(m_indices.size() * sizeof(IndexType), m_indices.data());
	}
	void release();
	Mesh(std::string a_name) :name(a_name) {}
	Mesh(std::string a_name, const std::vector<Vertex> vert, const std::vector<IndexType> indices)
		:name(a_name), m_vertices(vert), m_indices(indices) {}
};

class Model
{
public:
	struct InternalModel
	{
		struct Data
		{
			glm::mat4 model;
		};
		void init() {}
		std::function<void()> update;
		Data data;
	};
	Model(uint32_t binding, ShaderType type) :m_buffer(binding, type) {}
	void init(const std::string& pathToPixels, TextureCreateInfo info, std::string name, std::string meshName);
	void update(uint32_t frame);
	void release()
	{
		m_textures.release();
		m_buffer.release();

		if (m_mesh.use_count() == 2) // inside model and static map in Mesh
		{
			m_mesh->release();
			Mesh::DeleteMesh(m_mesh->name);
		}
		else
			m_mesh.reset();
	}
	VkDescriptorBufferInfo* getDescriptorBufferInfo(uint32_t index)
	{
		return m_buffer.getDescriptorBufferInfo(index);
	}

	VkDescriptorImageInfo* getDescriptorImageInfo(uint32_t index)
	{
		return m_textures[index].getDescriptorImageInfo();
	}

	Buffer& getBuffer(uint32_t index) { return m_buffer[index]; }
	TextureImage& getImage(uint32_t index) { return m_textures[index]; }

	std::shared_ptr<Mesh> m_mesh;
	FrameResources<TextureImage> m_textures;
	UniformBuffer<InternalModel> m_buffer;
	glm::vec3 m_position{};
	std::string m_name;
};

