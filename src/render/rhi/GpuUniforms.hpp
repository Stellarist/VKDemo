#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

struct GpuUniforms {
};

struct GpuTransform : public GpuUniforms {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	static vk::DescriptorSetLayoutBinding binding(uint32_t binding = {});
};
