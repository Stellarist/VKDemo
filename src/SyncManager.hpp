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

	std::pair<vk::Semaphore, uint32_t> allocateSemaphore();
	std::pair<vk::Fence, uint32_t>     allocateFence();

	vk::Semaphore getSemaphore(uint32_t index);
	vk::Fence     getFence(uint32_t index);

	vk::Result waitForFence(vk::Fence fence, uint64_t timeout = std::numeric_limits<uint64_t>::max());
	vk::Result waitForFences(std::span<const vk::Fence> fences, bool wait_all, uint64_t timeout = std::numeric_limits<uint64_t>::max());

	void resetFence(vk::Fence fence);
	void resetFences(std::span<const vk::Fence> fences);
};
