#include "Mesh.hpp"

vk::VertexInputBindingDescription Vertex::binding()
{
	return {
	    0,
	    sizeof(Vertex),
	    vk::VertexInputRate::eVertex,
	};
}

std::vector<vk::VertexInputAttributeDescription> Vertex::attributes()
{
	return {
	    {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)},
	    {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
	    {2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
	    {3, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, color)},
	};
}

vk::DescriptorSetLayoutBinding Transform::binding()
{
	return {
	    0,
	    vk::DescriptorType::eUniformBuffer,
	    1,
	    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
	};
}
