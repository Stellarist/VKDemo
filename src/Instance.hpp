#pragma once

#include <vulkan/vulkan.hpp>

class Instance {
private:
	vk::Instance instance{};

	std::vector<const char*> getExtensions();

public:
	Instance();
	~Instance();

	vk::Instance get() const;
};
