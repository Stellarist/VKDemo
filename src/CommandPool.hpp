#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class CommandPool {
private:
	vk::CommandPool command_pool;

	Context& context;

public:
	CommandPool(Context& context);
	~CommandPool();

	vk::CommandPool get() const;
};
