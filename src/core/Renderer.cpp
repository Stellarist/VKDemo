#include "Renderer.hpp"

#include <print>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Mesh.hpp"

constexpr std::array<Vertex, 3> vertices = {
    Vertex{glm::vec3(0.0, -0.5, 0.0)},
    Vertex{glm::vec3(-0.5, 0.5, 0.0)},
    Vertex{glm::vec3(0.5, 0.5, 0.0)},
};

Renderer::Renderer(Window& window)
{
	context = std::make_unique<Context>(window);
	swap_chain = std::make_unique<SwapChain>(window, *context);
	render_pass = std::make_unique<RenderPass>(*context, *swap_chain);
	graphics_pipeline = std::make_unique<GraphicsPipeline>(*context, *render_pass);
	command_manager = std::make_unique<CommandManager>(*context);
	sync_manager = std::make_unique<SyncManager>(*context);
	vertex_buffer = std::make_unique<Buffer>(*context, sizeof(vertices), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	vertex_buffer->upload(static_cast<void*>(const_cast<Vertex*>(vertices.data())), vertices.size() * sizeof(Vertex));

	frame.command_buffer_index = command_manager->allocateBuffer();
	frame.wait_semaphore_index = sync_manager->allocateSemaphores(2).first;
	frame.signal_semaphore_index = frame.wait_semaphore_index + 1;
	frame.fence_index = sync_manager->allocateFences(2).first;
}

void Renderer::begin()
{
	auto wait = sync_manager->getSemaphore(frame.wait_semaphore_index);
	auto fence = sync_manager->getFence(frame.fence_index);
	auto command = command_manager->getBuffer(frame.command_buffer_index);

	sync_manager->waitForFence(fence);
	sync_manager->resetFence(fence);

	frame.image_index = swap_chain->acquireNextImage(wait, nullptr);
	command.reset();

	command_manager->begin(command);
	render_pass->begin(command, frame.image_index, swap_chain->getExtent(), {{0.0f, 0.0f, 0.0f, 1.0f}});
	command.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline->get());

	vk::Rect2D scissor{};
	scissor.setOffset({0, 0})
	    .setExtent(swap_chain->getExtent());

	vk::Viewport viewport{};
	viewport.setX(0.0f)
	    .setY(0.0f)
	    .setWidth(static_cast<float>(swap_chain->getExtent().width))
	    .setHeight(static_cast<float>(swap_chain->getExtent().height))
	    .setMinDepth(0.0f)
	    .setMaxDepth(1.0f);

	command.setViewport(0, viewport);
	command.setScissor(0, scissor);
}

void Renderer::end()
{
	auto command = command_manager->getBuffer(frame.command_buffer_index);
	auto wait = sync_manager->getSemaphore(frame.wait_semaphore_index);
	auto signal = sync_manager->getSemaphore(frame.signal_semaphore_index);
	auto fence = sync_manager->getFence(frame.fence_index);

	render_pass->end(command);
	command_manager->end(command);

	vk::SubmitInfo         submit_info{};
	vk::PipelineStageFlags waitstages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submit_info.setCommandBuffers(command)
	    .setWaitDstStageMask(waitstages)
	    .setWaitSemaphores(wait)
	    .setSignalSemaphores(signal);
	context->getGraphicsQueue().submit(submit_info, fence);

	vk::SwapchainKHR   swapchain = swap_chain->get();
	vk::PresentInfoKHR present_info{};
	present_info.setImageIndices(frame.image_index)
	    .setSwapchains(swapchain)
	    .setWaitSemaphores(signal);
	if (context->getGraphicsQueue().presentKHR(present_info) != vk::Result::eSuccess)
		std::println("Failed to present swap chain image");

	frame.fence_index = (frame.fence_index + 1) % sync_manager->fenceCount();
}

void Renderer::render()
{
	auto command = command_manager->getBuffer(frame.command_buffer_index);
	command.bindVertexBuffers(0, vertex_buffer->get(), {0});
	command.draw(vertices.size(), 1, 0, 0);
}

void Renderer::wait()
{
	context->getLogicalDevice().waitIdle();
}
