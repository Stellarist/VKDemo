#include "GpuUniforms.hpp"

vk::DescriptorSetLayoutBinding GpuTransform::binding(uint32_t binding)
{
	return {
	    binding,
	    vk::DescriptorType::eUniformBuffer,
	    1,
	    vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
	};
}