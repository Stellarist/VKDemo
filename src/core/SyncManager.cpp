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

uint32_t SyncManager::allocateSemaphore()
{
	return allocateSemaphores(1).first;
}

std::pair<uint32_t, uint32_t> SyncManager::allocateSemaphores(uint32_t count)
{
	auto start = semaphore_pool.size();

	for (uint32_t i = 0; i < count; i++) {
		vk::SemaphoreCreateInfo semaphore_info{};

		vk::Semaphore semaphore = context->getLogicalDevice().createSemaphore(semaphore_info);
		semaphore_pool.push_back(std::move(semaphore));
	}

	return {start, semaphore_pool.size()};
}

uint32_t SyncManager::allocateFence()
{
	return allocateFences(1).first;
}

std::pair<uint32_t, uint32_t> SyncManager::allocateFences(uint32_t count)
{
	auto start = fence_pool.size();

	for (uint32_t i = 0; i < count; i++) {
		vk::FenceCreateInfo fence_info{};
		fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

		vk::Fence fence = context->getLogicalDevice().createFence(fence_info);
		fence_pool.push_back(std::move(fence));
	}

	return {start, fence_pool.size()};
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

uint32_t SyncManager::semaphoreCount() const
{
	return static_cast<uint32_t>(semaphore_pool.size());
}

uint32_t SyncManager::fenceCount() const
{
	return static_cast<uint32_t>(fence_pool.size());
}
