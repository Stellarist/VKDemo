#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class CommandManager {
private:
	vk::CommandPool                command_pool;
	std::vector<vk::CommandBuffer> command_buffers;

	Context* context{};

public:
	CommandManager(Context& context);
	~CommandManager();

	void createPool();

	vk::CommandBuffer              allocateBuffer();
	std::vector<vk::CommandBuffer> allocateBuffers(uint32_t count);

	void freeBuffer(vk::CommandBuffer buffer);
	void freeBuffers(std::span<const vk::CommandBuffer> buffers);

	void begin(vk::CommandBuffer command_buffer);
	void end(vk::CommandBuffer command_buffer);

	void resetPool(vk::CommandPoolResetFlags flags = {});
};
