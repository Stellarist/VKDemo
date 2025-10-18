#include "GpuVertex.hpp"

vk::VertexInputBindingDescription GpuVertex::binding(uint32_t binding)
{
	return {
	    binding,
	    sizeof(GpuVertex),
	    vk::VertexInputRate::eVertex,
	};
}

std::vector<vk::VertexInputAttributeDescription> GpuVertex::attributes(uint32_t binding)
{
	return {
	    {0, binding, vk::Format::eR32G32B32Sfloat, offsetof(GpuVertex, pos)},
	    {1, binding, vk::Format::eR32G32B32Sfloat, offsetof(GpuVertex, normal)},
	    {2, binding, vk::Format::eR32G32Sfloat, offsetof(GpuVertex, uv)},
	    {3, binding, vk::Format::eR32G32B32A32Sfloat, offsetof(GpuVertex, color)},
	};
}
