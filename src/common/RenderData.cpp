#include "RenderData.hpp"

vk::VertexInputBindingDescription Vertex::binding(uint32_t binding)
{
	return {
	    binding,
	    sizeof(Vertex),
	    vk::VertexInputRate::eVertex,
	};
}

std::vector<vk::VertexInputAttributeDescription> Vertex::attributes(uint32_t binding)
{
	return {
	    {0, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)},
	    {1, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
	    {2, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
	    {3, binding, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, color)},
	};
}

vk::DescriptorSetLayoutBinding Transform::binding(uint32_t binding)
{
	return {
	    binding,
	    vk::DescriptorType::eUniformBuffer,
	    1,
	    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
	};
}
