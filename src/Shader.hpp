#pragma once

#include <unordered_map>

#include <vulkan/vulkan.hpp>

class Shader {
private:
	vk::ShaderModule shader;
	vk::Device       device;

	std::string filename;

	std::vector<std::byte> codes;

	std::unordered_map<vk::ShaderStageFlagBits, std::string> stages;

public:
	Shader(vk::Device device, std::string_view filename);
	~Shader();

	void read();
	void create();

	void                                           setStage(vk::ShaderStageFlagBits stage, std::string_view entry);
	vk::PipelineShaderStageCreateInfo              getStage(vk::ShaderStageFlagBits stage) const;
	std::vector<vk::PipelineShaderStageCreateInfo> getStages() const;

	vk::ShaderModule get() const;
};
