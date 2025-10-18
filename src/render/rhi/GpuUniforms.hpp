#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct GpuTransform {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	static vk::DescriptorSetLayoutBinding binding(uint32_t binding = {});
};
