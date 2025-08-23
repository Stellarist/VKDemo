#pragma once

#include <functional>

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

	vk::CommandBuffer            allocateBuffer();
	std::span<vk::CommandBuffer> allocateBuffers(uint32_t count);

	void freeBuffer(vk::CommandBuffer buffer);
	void freeBuffers(std::span<vk::CommandBuffer> buffers);

	void executeCommand(vk::Queue queue, std::function<void(vk::CommandBuffer)> func);

	void resetPool(vk::CommandPoolResetFlags flags = {});

	vk::CommandPool getPool() const;
};
