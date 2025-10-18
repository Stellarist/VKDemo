#pragma once

#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

struct GPUVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;

	static vk::VertexInputBindingDescription                binding(uint32_t binding = {});
	static std::vector<vk::VertexInputAttributeDescription> attributes(uint32_t binding = {});
};

struct GPUTransform {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	static vk::DescriptorSetLayoutBinding binding(uint32_t binding = {});
};
