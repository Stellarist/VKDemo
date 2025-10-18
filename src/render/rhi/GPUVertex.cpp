#include "GPUVertex.hpp"

vk::VertexInputBindingDescription GPUVertex::binding(uint32_t binding)
{
	return {
	    binding,
	    sizeof(GPUVertex),
	    vk::VertexInputRate::eVertex,
	};
}

std::vector<vk::VertexInputAttributeDescription> GPUVertex::attributes(uint32_t binding)
{
	return {
	    {0, binding, vk::Format::eR32G32B32Sfloat, offsetof(GPUVertex, pos)},
	    {1, binding, vk::Format::eR32G32B32Sfloat, offsetof(GPUVertex, normal)},
	    {2, binding, vk::Format::eR32G32Sfloat, offsetof(GPUVertex, uv)},
	    {3, binding, vk::Format::eR32G32B32A32Sfloat, offsetof(GPUVertex, color)},
	};
}

vk::DescriptorSetLayoutBinding GPUTransform::binding(uint32_t binding)
{
	return {
	    binding,
	    vk::DescriptorType::eUniformBuffer,
	    1,
	    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
	};
}
