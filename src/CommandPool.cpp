#include "CommandPool.hpp"

CommandPool::CommandPool(Device& device) :
    device(device)
{
	vk::CommandPoolCreateInfo pool_info{};
	pool_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	    .setQueueFamilyIndex(device.getQueueFamilyIndices().graphics_family.value());

	command_pool = device.getLogicalDevice().createCommandPool(pool_info);
}

CommandPool::~CommandPool()
{
	device.getLogicalDevice().destroyCommandPool(command_pool);
}

vk::CommandPool CommandPool::get() const
{
	return command_pool;
}
