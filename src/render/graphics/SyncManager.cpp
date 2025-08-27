#include "SyncManager.hpp"

#include <vulkan/vulkan.h>

SyncManager::SyncManager(Context& context) :
    context(&context)
{}

SyncManager::~SyncManager()
{
	for (auto& semaphore : semaphore_pool)
		context->getLogicalDevice().destroySemaphore(semaphore);
	for (auto& fence : fence_pool)
		context->getLogicalDevice().destroyFence(fence);
}

vk::Semaphore SyncManager::allocateSemaphore()
{
	vk::SemaphoreCreateInfo semaphore_info{};

	vk::Semaphore semaphore = context->getLogicalDevice().createSemaphore(semaphore_info);
	semaphore_pool.push_back(semaphore);

	return semaphore;
}

vk::Fence SyncManager::allocateFence()
{
	vk::FenceCreateInfo fence_info{};
	fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

	vk::Fence fence = context->getLogicalDevice().createFence(fence_info);
	fence_pool.push_back(fence);

	return fence;
}

vk::Fence SyncManager::nextFence(vk::Fence fence)
{
	auto it = std::find(fence_pool.begin(), fence_pool.end(), fence);
	if (it == fence_pool.end())
		throw std::invalid_argument("Invalid fence");

	auto index = std::distance(fence_pool.begin(), it);
	index = (index + 1) % fence_pool.size();

	return fence_pool.at(index);
}

void SyncManager::waitForFence(vk::Fence fence, uint64_t timeout)
{
	if (context->getLogicalDevice().waitForFences(fence, vk::True, timeout) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to wait for fence");
}

void SyncManager::resetFence(vk::Fence fence)
{
	context->getLogicalDevice().resetFences(fence);
}
