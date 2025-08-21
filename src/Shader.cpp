#include "Shader.hpp"

#include <fstream>

Shader::Shader(const std::filesystem::path& filepath, Device& device) :
    device(device)
{
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw std::runtime_error("Failed to open shader file: " + filepath.string());

	std::vector<std::byte> code(file.tellg());
	file.seekg(0);
	file.read(reinterpret_cast<char*>(code.data()), code.size());
	if (file.fail())
		throw std::runtime_error("Failed to read shader file: " + filepath.string());

	vk::ShaderModuleCreateInfo create_info{};
	create_info.setCodeSize(code.size())
	    .setPCode(reinterpret_cast<const uint32_t*>(code.data()));
	shader = device.getLogicalDevice().createShaderModule(create_info);
	if (!shader)
		throw std::runtime_error("Failed to create shader module");
}

Shader::~Shader()
{
	device.getLogicalDevice().destroyShaderModule(shader);
}

vk::ShaderModule Shader::get() const
{
	return shader;
}
