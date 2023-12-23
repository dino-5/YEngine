#include "Model.h"
#include "GraphicsModule.h"
#include "ImGui.h"

using namespace graphics;
std::shared_ptr<Mesh> Mesh::GetMesh(std::string a_name)
{
	if (s_meshes.find(a_name) == s_meshes.end())
		return nullptr;
	return s_meshes[a_name];
}

std::shared_ptr<Mesh> Mesh::CreateMesh(std::string a_name, const std::string& pathToModel)
{
	if (s_meshes.find(a_name) == s_meshes.end())
	{
		s_meshes[a_name] = std::make_shared<Mesh>(a_name);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathToModel.c_str())) {
			throw std::runtime_error(warn + err);
		}
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Geometry::Vertex vertex{};
				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};

				s_meshes[a_name]->m_vertices.push_back(vertex);
				s_meshes[a_name]->m_indices.push_back(s_meshes[a_name]->m_indices.size());
			}
		}
		s_meshes[a_name]->initBuffers();
	}
	return s_meshes[a_name];
}
std::shared_ptr<Mesh> Mesh::CreateMesh(std::string a_name, const std::vector<Geometry::Vertex> vertices,
	const std::vector<Geometry::IndexType> indices)
{
	if (s_meshes.find(a_name) == s_meshes.end())
	{
		s_meshes[a_name] = std::make_shared<Mesh>(a_name, vertices, indices);
		s_meshes[a_name]->initBuffers();
	}
	return s_meshes[a_name];
}

void Mesh::release()
{
	m_indexBuffer.release();
	m_vertexBuffer.release();
}

void Mesh::Cleanup()
{
	for (auto& obj : s_meshes)
		obj.second->release();
}

void Mesh::DeleteMesh(std::string name)
{
	auto it = s_meshes.find(name);
	if (it != s_meshes.end())
	{
		s_meshes.erase(it);
	}
}

void Model::init(const std::string& pathToPixels, TextureCreateInfo info,
	std::string name, std::string meshName)
{
	m_name = name;
	for(auto& texture: m_texture)
		texture.init(pathToPixels, info);

	m_mesh = Mesh::GetMesh(meshName);

	ImGuiEntry entry;
	entry.address = &m_position[0];
	entry.name = m_name.c_str();
	entry.type = ImGuiType::FLOAT3;
	entry.min = -10;
	entry.max =  10;
	ImGuiManager::AddImGuiEntry(entry);

	m_buffer.init();
	m_buffer.m_data.update = [&]() {
		glm::mat4 mat = glm::mat4(1.0f);
		m_buffer.m_data.data.model = glm::translate(mat, m_position);
	};
}

void Model::update(uint32_t frame)
{
	m_buffer.update(frame);
}
