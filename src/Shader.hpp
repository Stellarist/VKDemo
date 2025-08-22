#pragma once

#include <filesystem>

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class Shader {
private:
	vk::ShaderModule shader;

	Context& context;

public:
	Shader(const std::filesystem::path& filepath, Context& context);
	~Shader();

	vk::ShaderModule get() const;
};
