#pragma once

#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;

	static constexpr vk::VertexInputBindingDescription                binding();
	static constexpr std::vector<vk::VertexInputAttributeDescription> attributes();
};

struct Index {
};
