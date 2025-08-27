#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class CommandManager {
private:
	std::pair<vk::CommandPool, std::vector<vk::CommandBuffer>> command_maps;

	Context* context{};

public:
	CommandManager(Context& context);
	~CommandManager();

	void createPool();

	vk::CommandBuffer              allocateBuffer();
	std::vector<vk::CommandBuffer> allocateBuffers(uint32_t count);

	void freeBuffer(vk::CommandBuffer buffer);
	void freeBuffers(std::span<const vk::CommandBuffer> buffers);

	void begin(vk::CommandBuffer command);
	void end(vk::CommandBuffer command);

	void resetPool(vk::CommandPoolResetFlags flags = {});
};
