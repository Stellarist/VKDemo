#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class CommandManager {
private:
	vk::CommandPool                command_pool;
	std::vector<vk::CommandBuffer> command_buffers;

	Context* context;

	void createPool();

public:
	CommandManager(Context& context);
	~CommandManager();

	std::pair<vk::CommandBuffer, uint32_t>            allocateBuffer();
	std::pair<std::span<vk::CommandBuffer>, uint32_t> allocateBuffers(uint32_t count);

	void freeBuffer(vk::CommandBuffer buffer);
	void freeBuffers(std::span<vk::CommandBuffer> buffers);

	void begin(vk::CommandBuffer command_buffer);
	void end(vk::CommandBuffer command_buffer);

	void resetPool(vk::CommandPoolResetFlags flags = {});

	vk::CommandPool   getPool() const;
	vk::CommandBuffer getBuffer(uint32_t index) const;
};
