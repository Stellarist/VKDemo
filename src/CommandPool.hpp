#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.hpp"

class CommandPool {
private:
	vk::CommandPool command_pool;

	Device& device;

public:
	CommandPool(Device& device);
	~CommandPool();

	vk::CommandPool get() const;
};
