#include "CommandManager.hpp"

CommandManager::CommandManager(Context& context) :
    context(&context)
{
	createPool();
}

CommandManager::~CommandManager()
{
	context->getLogicalDevice().destroyCommandPool(command_pool);
}

void CommandManager::createPool()
{
	vk::CommandPoolCreateInfo pool_info{};
	pool_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	    .setQueueFamilyIndex(context->getGraphicsQueueIndex());

	command_pool = context->getLogicalDevice().createCommandPool(pool_info);
}

uint32_t CommandManager::allocateBuffer()
{
	return allocateBuffers(1).first;
}

std::pair<uint32_t, uint32_t> CommandManager::allocateBuffers(uint32_t count)
{
	uint32_t start = command_buffers.size();
	command_buffers.reserve(start + count);

	vk::CommandBufferAllocateInfo alloc_info{};
	alloc_info.setCommandPool(command_pool)
	    .setLevel(vk::CommandBufferLevel::ePrimary)
	    .setCommandBufferCount(count);

	std::vector<vk::CommandBuffer> buffers{};
	buffers = context->getLogicalDevice().allocateCommandBuffers(alloc_info);
	command_buffers.insert(command_buffers.end(), buffers.begin(), buffers.end());

	return {start, command_buffers.size()};
}

void CommandManager::freeBuffer(vk::CommandBuffer buffer)
{
	freeBuffers(std::span<vk::CommandBuffer>{&buffer, 1});
}

void CommandManager::freeBuffers(std::span<vk::CommandBuffer> buffers)
{
	context->getLogicalDevice().freeCommandBuffers(command_pool, buffers);
	std::erase_if(command_buffers, [&](const vk::CommandBuffer& buffer) {
		return std::find(buffers.begin(), buffers.end(), buffer) != buffers.end();
	});
}

void CommandManager::submit(vk::CommandBuffer                       command,
                            std::span<const vk::Semaphore>          wait_semaphores,
                            std::span<const vk::Semaphore>          signal_semaphores,
                            std::span<const vk::PipelineStageFlags> wait_stages,
                            vk::Fence                               fence)
{
	vk::SubmitInfo submit_info{};
	submit_info.setCommandBuffers(command)
	    .setWaitSemaphores(wait_semaphores)
	    .setSignalSemaphores(signal_semaphores)
	    .setWaitDstStageMask(wait_stages);

	context->getGraphicsQueue().submit(submit_info, fence);
}

void CommandManager::present(vk::CommandBuffer                 command,
                             std::span<const uint32_t>         image_indices,
                             std::span<const vk::SwapchainKHR> swap_chains,
                             std::span<const vk::Semaphore>    wait_semaphores)
{
	vk::PresentInfoKHR present_info{};
	present_info.setImageIndices(image_indices)
	    .setSwapchains(swap_chains)
	    .setWaitSemaphores(wait_semaphores);

	if (context->getPresentQueue().presentKHR(present_info) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present swap chain image");
}

void CommandManager::begin(vk::CommandBuffer command_buffer)
{
	vk::CommandBufferBeginInfo begin_info{};
	begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	command_buffer.begin(begin_info);
}

void CommandManager::end(vk::CommandBuffer command_buffer)
{
	command_buffer.end();
}

void CommandManager::resetPool(vk::CommandPoolResetFlags flags)
{
	context->getLogicalDevice().resetCommandPool(command_pool, flags);
	command_buffers.clear();
}

vk::CommandPool CommandManager::getPool() const
{
	return command_pool;
}

vk::CommandBuffer CommandManager::getBuffer(uint32_t index) const
{
	return command_buffers.at(index);
}
