#include "CommandManager.hpp"

CommandManager::CommandManager(Context& context) :
    context(&context)
{
	createPool();
}

CommandManager::~CommandManager()
{
	context->getLogicalDevice().destroyCommandPool(command_maps.first);
}

void CommandManager::createPool()
{
	vk::CommandPoolCreateInfo pool_info{};
	pool_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	    .setQueueFamilyIndex(context->getGraphicsQueueIndex());

	command_maps.first = context->getLogicalDevice().createCommandPool(pool_info);
}

vk::CommandBuffer CommandManager::allocateBuffer()
{
	return allocateBuffers(1).front();
}

std::vector<vk::CommandBuffer> CommandManager::allocateBuffers(uint32_t count)
{
	command_maps.second.reserve(command_maps.second.size() + count);

	vk::CommandBufferAllocateInfo alloc_info{};
	alloc_info.setCommandPool(command_maps.first)
	    .setLevel(vk::CommandBufferLevel::ePrimary)
	    .setCommandBufferCount(count);

	auto buffers = context->getLogicalDevice().allocateCommandBuffers(alloc_info);
	command_maps.second.insert(command_maps.second.end(), buffers.begin(), buffers.end());

	return buffers;
}

void CommandManager::freeBuffer(vk::CommandBuffer buffer)
{
	freeBuffers({&buffer, 1});
}

void CommandManager::freeBuffers(std::span<const vk::CommandBuffer> buffers)
{
	context->getLogicalDevice().freeCommandBuffers(command_maps.first, buffers);
	std::erase_if(command_maps.second, [&](const vk::CommandBuffer& buffer) {
		return std::find(buffers.begin(), buffers.end(), buffer) != buffers.end();
	});
}

void CommandManager::begin(vk::CommandBuffer command)
{
	vk::CommandBufferBeginInfo begin_info{};
	begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	command.begin(begin_info);
}

void CommandManager::end(vk::CommandBuffer command)
{
	command.end();
}

void CommandManager::resetPool(vk::CommandPoolResetFlags flags)
{
	context->getLogicalDevice().resetCommandPool(command_maps.first, flags);
	command_maps.second.clear();
}
