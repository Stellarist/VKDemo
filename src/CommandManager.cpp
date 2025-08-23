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

vk::CommandBuffer CommandManager::allocateBuffer()
{
	return allocateBuffers(1).front();
}

std::span<vk::CommandBuffer> CommandManager::allocateBuffers(uint32_t count)
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

	return {command_buffers.data() + buffer_size, count};
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

void CommandManager::executeCommand(vk::Queue queue, std::function<void(vk::CommandBuffer)> func)
{
	auto command_buffer = allocateBuffer();

	vk::CommandBufferBeginInfo begin_info{};
	begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	command_buffer.begin(begin_info);
	func(command_buffer);
	command_buffer.end();

	vk::SubmitInfo submit_info{};
	submit_info.setCommandBuffers({command_buffer});
	queue.submit(submit_info, {});
	queue.waitIdle();

	freeBuffer(command_buffer);
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

void CommandBuffer::record(uint32_t image_index)
{
	vk::CommandBufferBeginInfo command_begin_info{};
	command_begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	vk::ClearValue clear_value{};
	clear_value.setColor({0.0f, 0.0f, 0.0f, 1.0f});

	vk::Viewport viewport{};
	viewport.setX(0)
	    .setY(0)
	    .setWidth(static_cast<float>(swap_chain.getExtent().width))
	    .setHeight(static_cast<float>(swap_chain.getExtent().height))
	    .setMinDepth(0.0f)
	    .setMaxDepth(1.0f);

	vk::Rect2D scissor{};
	scissor.setOffset({0, 0})
	    .setExtent(swap_chain.getExtent());

	vk::RenderPassBeginInfo pass_begin_info{};
	pass_begin_info.setRenderPass(render_pass.get())
	    .setFramebuffer(render_pass.getFrameBuffers()[image_index])
	    .setRenderArea(scissor)
	    .setClearValueCount(1)
	    .setPClearValues(&clear_value);

	command_buffer.begin(command_begin_info);
	command_buffer.beginRenderPass(pass_begin_info, vk::SubpassContents::eInline);
	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline.get());
	command_buffer.setViewport(0, viewport);
	command_buffer.setScissor(0, scissor);
	command_buffer.draw(3, 1, 0, 0);
	command_buffer.endRenderPass();
	command_buffer.end();
}
