#include "CommandPool.hpp"

CommandPool::CommandPool(Context& context) :
    context(context)
{
	vk::CommandPoolCreateInfo pool_info{};
	pool_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	    .setQueueFamilyIndex(context.getQueueFamilyIndices().graphics_family.value());

	command_pool = context.getLogicalDevice().createCommandPool(pool_info);
}

CommandPool::~CommandPool()
{
	context.getLogicalDevice().destroyCommandPool(command_pool);
}

vk::CommandPool CommandPool::get() const
{
	return command_pool;
}
