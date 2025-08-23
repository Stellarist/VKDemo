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
	    .setQueueFamilyIndex(context->getQueueFamilyIndices().graphics_family.value());

	command_pool = context->getLogicalDevice().createCommandPool(pool_info);
}

std::pair<vk::CommandBuffer, uint32_t> CommandManager::allocateBuffer()
{
	auto [buffer, index] = allocateBuffers(1);
	return {buffer.front(), index};
}

std::pair<std::span<vk::CommandBuffer>, uint32_t> CommandManager::allocateBuffers(uint32_t count)
{
	size_t buffer_size = command_buffers.size();
	command_buffers.reserve(buffer_size + count);

	vk::CommandBufferAllocateInfo alloc_info{};
	alloc_info.setCommandPool(command_pool)
	    .setLevel(vk::CommandBufferLevel::ePrimary)
	    .setCommandBufferCount(count);

	std::vector<vk::CommandBuffer> buffers{};
	buffers = context->getLogicalDevice().allocateCommandBuffers(alloc_info);
	std::move(buffers.begin(), buffers.end(), std::back_inserter(command_buffers));

	return {{command_buffers.data() + buffer_size, count}, buffer_size};
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
