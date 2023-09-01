#include "Geometry.h"

namespace Geometry
{

const std::vector<Vertex> GetDefaultVertices()
{
	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	return vertices;
}

const std::vector<IndexType> GetDefaultIndices()
{
	const std::vector<IndexType> indices = {
		0, 1, 2, 2, 3, 0
	};
	return indices;
}

}
