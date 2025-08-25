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

vk::CommandBuffer CommandManager::allocateBuffer()
{
	return allocateBuffers(1).front();
}

std::vector<vk::CommandBuffer> CommandManager::allocateBuffers(uint32_t count)
{
	command_buffers.reserve(command_buffers.size() + count);

	vk::CommandBufferAllocateInfo alloc_info{};
	alloc_info.setCommandPool(command_pool)
	    .setLevel(vk::CommandBufferLevel::ePrimary)
	    .setCommandBufferCount(count);

	auto buffers = context->getLogicalDevice().allocateCommandBuffers(alloc_info);
	command_buffers.insert(command_buffers.end(), buffers.begin(), buffers.end());

	return buffers;
}

void CommandManager::freeBuffer(vk::CommandBuffer buffer)
{
	freeBuffers({&buffer, 1});
}

void CommandManager::freeBuffers(std::span<const vk::CommandBuffer> buffers)
{
	context->getLogicalDevice().freeCommandBuffers(command_pool, buffers);
	std::erase_if(command_buffers, [&](const vk::CommandBuffer& buffer) {
		return std::find(buffers.begin(), buffers.end(), buffer) != buffers.end();
	});
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
