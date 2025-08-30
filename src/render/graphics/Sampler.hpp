#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class Sampler {
	vk::Sampler sampler;

	Context* context{};

public:
	Sampler(Context& context);
	~Sampler();

	void create();

	vk::Sampler get() const;

	// move to shader
	static vk::DescriptorSetLayoutBinding binding(uint32_t binding = {});
};
