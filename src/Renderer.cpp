#include "Renderer.hpp"

#include <print>

#include <vulkan/vulkan.hpp>

constexpr int SCR_WIDTH = 2560;
constexpr int SCR_HEIGHT = 1440;

Renderer::Renderer() :
    window("VKDemo", SCR_WIDTH, SCR_HEIGHT),
    context(window),
    swap_chain(window, context),
    render_pass(context, swap_chain),
    graphics_pipeline(context, render_pass),
    command_manager(context),
    sync_manager(context)
{}

void Renderer::run()
{
	bool should_close = false;
	while (!should_close) {
		static uint64_t last_tick{}, current_tick{};
		current_tick = SDL_GetTicks();
		float delta_time = static_cast<float>(current_tick - last_tick) / 1000.0f;
		last_tick = current_tick;

		begin();
		render();
		end();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EventType::SDL_EVENT_QUIT:
				should_close = true;
				break;
			case SDL_EventType::SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE) {
					should_close = true;
					break;
				}
			}
		}

		context.getLogicalDevice().waitIdle();
	}
}

void Renderer::begin()
{
	auto [wait_s, wait_i] = sync_manager.allocateSemaphore();
	auto [signal_s, signal_i] = sync_manager.allocateSemaphore();
	auto [fence, fence_i] = sync_manager.allocateFence();
	auto [command, command_i] = command_manager.allocateBuffer();

	wait_semaphore_index = wait_i;
	signal_semaphore_index = signal_i;
	command_buffer_index = command_i;
	fence_index = fence_i;

	sync_manager.waitForFence(fence);
	sync_manager.resetFence(fence);
	image_index = swap_chain.acquireNextImage(wait_s, fence);
	command.reset();

	command_manager.begin(command);
	render_pass.begin(command, image_index, swap_chain.getExtent(), {{0.0f, 0.0f, 0.0f, 1.0f}});
	command.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline.get());

	vk::Rect2D scissor{};
	scissor.setOffset({0, 0})
	    .setExtent(swap_chain.getExtent());

	vk::Viewport viewport{};
	viewport.setX(0.0f)
	    .setY(0.0f)
	    .setWidth(static_cast<float>(swap_chain.getExtent().width))
	    .setHeight(static_cast<float>(swap_chain.getExtent().height))
	    .setMinDepth(0.0f)
	    .setMaxDepth(1.0f);

	command.setViewport(0, viewport);
	command.setScissor(0, scissor);
}

void Renderer::end()
{
	auto command = command_manager.getBuffer(command_buffer_index);
	auto wait = sync_manager.getSemaphore(wait_semaphore_index);
	auto signal = sync_manager.getSemaphore(signal_semaphore_index);
	auto fence = sync_manager.getFence(fence_index);

	render_pass.end(command);
	command_manager.end(command);

	vk::SubmitInfo         submit_info{};
	vk::PipelineStageFlags waitstages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submit_info.setCommandBuffers(command)
	    .setWaitDstStageMask(waitstages)
	    .setWaitSemaphores(wait)
	    .setSignalSemaphores(signal);
	context.getGraphicsQueue().submit(submit_info, fence);

	vk::SwapchainKHR   swapchain = swap_chain.get();
	vk::PresentInfoKHR present_info{};
	present_info.setImageIndices(image_index)
	    .setSwapchains(swapchain)
	    .setWaitSemaphores(signal);
	if (context.getGraphicsQueue().presentKHR(present_info) != vk::Result::eSuccess)
		std::println("Failed to present swap chain image");
}

void Renderer::render()
{
	auto command = command_manager.getBuffer(command_buffer_index);
	command.draw(3, 1, 0, 0);
}
