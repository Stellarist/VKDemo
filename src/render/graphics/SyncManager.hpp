#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class SyncManager {
private:
	std::vector<vk::Semaphore> semaphore_pool;
	std::vector<vk::Fence>     fence_pool;

	Context* context;

public:
	SyncManager(Context& context);
	~SyncManager();

	vk::Semaphore allocateSemaphore();
	vk::Fence     allocateFence();

	vk::Fence nextFence(vk::Fence fence);

	uint32_t semaphoreIndex(vk::Semaphore semaphore);
	uint32_t fenceIndex(vk::Fence fence);

	uint32_t semaphoreCount();
	uint32_t fenceCount();

	void waitForFence(vk::Fence fence, uint64_t timeout = std::numeric_limits<uint64_t>::max());
	void resetFence(vk::Fence fence);
};
