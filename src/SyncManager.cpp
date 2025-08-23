#include "SyncManager.hpp"

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

std::pair<vk::Semaphore, uint32_t> SyncManager::allocateSemaphore()
{
	vk::SemaphoreCreateInfo semaphore_info{};

	vk::Semaphore semaphore = context->getLogicalDevice().createSemaphore(semaphore_info);
	semaphore_pool.push_back(semaphore);

	return {semaphore, semaphore_pool.size() - 1};
}

std::pair<vk::Fence, uint32_t> SyncManager::allocateFence()
{
	vk::FenceCreateInfo fence_info{};
	fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

	vk::Fence fence = context->getLogicalDevice().createFence(fence_info);
	fence_pool.push_back(fence);

	return {fence, fence_pool.size() - 1};
}

vk::Semaphore SyncManager::getSemaphore(uint32_t index)
{
	return semaphore_pool.at(index);
}

vk::Fence SyncManager::getFence(uint32_t index)
{
	return fence_pool.at(index);
}

vk::Result SyncManager::waitForFence(vk::Fence fence, uint64_t timeout)
{
	return context->getLogicalDevice().waitForFences(fence, vk::True, timeout);
}

vk::Result SyncManager::waitForFences(std::span<const vk::Fence> fences, bool wait_all, uint64_t timeout)
{
	return context->getLogicalDevice().waitForFences(fences, wait_all, timeout);
}

void SyncManager::resetFence(vk::Fence fence)
{
	context->getLogicalDevice().resetFences(fence);
}

void SyncManager::resetFences(std::span<const vk::Fence> fences)
{
	context->getLogicalDevice().resetFences(fences);
}
