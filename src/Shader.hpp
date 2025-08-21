#pragma once

#include <filesystem>

#include <vulkan/vulkan.hpp>

#include "Device.hpp"

class Shader {
private:
	vk::ShaderModule shader;

	Device& device;

public:
	Shader(const std::filesystem::path& filepath, Device& device);
	~Shader();

	vk::ShaderModule get() const;
};
