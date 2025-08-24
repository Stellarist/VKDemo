#include "Mesh.hpp"

constexpr vk::VertexInputBindingDescription Vertex::binding()
{
	return {
	    0,
	    sizeof(Vertex),
	    vk::VertexInputRate::eVertex,
	};
}

constexpr std::vector<vk::VertexInputAttributeDescription> Vertex::attributes()
{
	return {
	    {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)},
	    {0, 1, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
	    {0, 2, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
	    {0, 3, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, color)},
	};
}
