#include "Model.h"
#include "GraphicsModule.h"
#include "ImGui.h"

using namespace graphics;

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
					attrib.vertices[3 * index.vertex_index + 2],
					attrib.vertices[3 * index.vertex_index + 1]
				};

				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 2],
					attrib.normals[3 * index.normal_index + 1],
				};

				s_meshes[a_name]->m_vertices.push_back(vertex);
				s_meshes[a_name]->m_indices.push_back(s_meshes[a_name]->m_indices.size());
			}
		}
		s_meshes[a_name]->initBuffers();
	}
	return s_meshes[a_name];
}

void Model::init(const std::string& pathToPixels, TextureCreateInfo info, const std::string& pathToModel,
	std::string name, std::string meshName)
{
	m_name = name;
	for(uint32_t i=0; i<3;i++)
		m_texture[i].init(pathToPixels, info);

	m_mesh = Mesh::CreateMesh(meshName, pathToModel);

	ImGuiEntry entry;
	entry.address = &m_position[0];
	entry.name = m_name.c_str();
	entry.type = ImGuiType::FLOAT3;
	entry.min = -10;
	entry.max =  10;
	ImGuiManager::AddImGuiEntry(entry);
}

void Model::update()
{
	m_uniformObject.model = glm::mat4(1.0f);
	m_uniformObject.model = glm::translate(m_uniformObject.model, m_position);
	m_uniformObject.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_uniformObject.proj = glm::perspective(glm::radians(45.0f), 
		graphics::GraphicsModule::GetInstance()->getSwapChain().getAspectRatio(), 0.1f, 100.0f);
	m_uniformObject.proj[1][1] *= -1;
}
