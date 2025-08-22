#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer(Context& context, SwapChain& swap_chain, RenderPass& render_pass, GraphicsPipeline& graphics_pipeline, CommandPool& command_pool) :
    context(context),
    swap_chain(swap_chain),
    render_pass(render_pass),
    graphics_pipeline(graphics_pipeline),
    command_pool(command_pool)
{
	vk::CommandBufferAllocateInfo alloc_info{};
	alloc_info.setCommandPool(command_pool.get())
	    .setLevel(vk::CommandBufferLevel::ePrimary)
	    .setCommandBufferCount(1);

	command_buffer = context.getLogicalDevice().allocateCommandBuffers(alloc_info).front();
}

CommandBuffer::~CommandBuffer()
{
	context.getLogicalDevice().freeCommandBuffers(command_pool.get(), command_buffer);
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

vk::CommandBuffer CommandBuffer::get() const
{
	return command_buffer;
}
