#pragma once

#include <glm/glm.hpp>

#include <vulkan/vulkan.hpp>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;

	static vk::VertexInputBindingDescription                binding();
	static std::vector<vk::VertexInputAttributeDescription> attributes();
};

template <typename Derived>
struct Uniform {
	static vk::DescriptorSetLayoutBinding binding()
	{
		Derived::binding();
	}
};

struct Transform : public Uniform<Transform> {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	static vk::DescriptorSetLayoutBinding binding();
};
