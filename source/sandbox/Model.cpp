#include "Model.h"
#include "graphics/GraphicsModule.h"

using namespace graphics;

void Model::init(const std::string& pathToPixels, TextureCreateInfo info, const std::string& pathToModel)
{
	VkDevice& device = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getDevice();
	VkPhysicalDevice& physicalDevice = GraphicsModule::GetInstance()->getDevice().getPhysicalDevice().getDevice();
	VkQueue& graphicsQueue = GraphicsModule::GetInstance()->getDevice().getLogicalDevice().getGraphicsQueue();
	VkCommandPool& cmdPool = GraphicsModule::GetInstance()->getCommandPool().getPool();
	for(uint32_t i=0; i<3;i++)
		m_texture[i].init(pathToPixels, info);

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

			vertex.color = { 1.0f, 1.0f, 1.0f };
			m_vertices.push_back(vertex);
			m_indices.push_back(m_indices.size());
		}
	}
}
