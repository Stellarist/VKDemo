#include "Renderer.hpp"

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "CommandManager.hpp"
#include "SyncManager.hpp"
#include "Mesh.hpp"

constexpr std::array<Vertex, 3> vertices = {
    Vertex{glm::vec3(0.0, -0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 0.0), glm::vec4(1.0, 1.0, .0, 1.0)},
    Vertex{glm::vec3(-0.5, 0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 1.0), glm::vec4(0.0, 1.0, 1.0, 1.0)},
    Vertex{glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec2(1.0, 1.0), glm::vec4(1.0, 0.0, 1.0, 1.0)},
};

Renderer::Renderer(Window& window)
{
	context = std::make_unique<Context>(window);
	swap_chain = std::make_unique<SwapChain>(window, *context);
	render_pass = std::make_unique<RenderPass>(*context, *swap_chain);
	graphics_pipeline = std::make_unique<GraphicsPipeline>(*context, *render_pass);

	staging_buffer = std::make_unique<Buffer>(*context, sizeof(vertices),
	                                          vk::BufferUsageFlagBits::eTransferSrc,
	                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	vertex_buffer = std::make_unique<Buffer>(*context, sizeof(vertices),
	                                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
	                                         vk::MemoryPropertyFlagBits::eDeviceLocal);
	staging_buffer->upload(static_cast<void*>(const_cast<Vertex*>(vertices.data())), vertices.size() * sizeof(Vertex));
	vertex_buffer->copyFrom(staging_buffer->get(), sizeof(vertices));

	frame.command_buffer_index = context->getCommandManager().allocateBuffer();
	frame.wait_semaphore_index = context->getSyncManager().allocateSemaphores(2).first;
	frame.signal_semaphore_index = frame.wait_semaphore_index + 1;
	frame.fence_index = context->getSyncManager().allocateFences(2).first;
}

void Renderer::render()
{
	auto command_manager = &context->getCommandManager();
	auto sync_manager = &context->getSyncManager();
	auto wait = sync_manager->getSemaphore(frame.wait_semaphore_index);
	auto signal = sync_manager->getSemaphore(frame.signal_semaphore_index);
	auto fence = sync_manager->getFence(frame.fence_index);
	auto command = command_manager->getBuffer(frame.command_buffer_index);
	auto chain = swap_chain->get();
	auto wait_stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	sync_manager->waitForFence(fence);
	sync_manager->resetFence(fence);

	frame.image_index = swap_chain->acquireNextImage(wait, nullptr);
	command.reset();

	command_manager->begin(command);
	render_pass->begin(command, frame.image_index, swap_chain->getExtent(), {{0.0f, 0.0f, 0.0f, 1.0f}});

	command.setScissor(0,
	                   vk::Rect2D{}
	                       .setOffset({0, 0})
	                       .setExtent(swap_chain->getExtent()));
	command.setViewport(0,
	                    vk::Viewport{}
	                        .setX(0.0f)
	                        .setY(0.0f)
	                        .setWidth(static_cast<float>(swap_chain->getExtent().width))
	                        .setHeight(static_cast<float>(swap_chain->getExtent().height))
	                        .setMinDepth(0.0f)
	                        .setMaxDepth(1.0f));

	draw();

	render_pass->end(command);
	command_manager->end(command);

	command_manager->submit(command, {&wait, 1}, {&signal, 1}, {&wait_stage, 1}, fence);
	command_manager->present(command, {&frame.image_index, 1}, {&chain, 1}, {&signal, 1});

	frame.fence_index = (frame.fence_index + 1) % sync_manager->fenceCount();
}

void Renderer::draw()
{
	auto command_manager = &context->getCommandManager();
	auto command = command_manager->getBuffer(frame.command_buffer_index);

	command.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline->get());
	command.bindVertexBuffers(0, vertex_buffer->get(), {0});
	command.draw(vertices.size(), 1, 0, 0);
}

void Renderer::wait()
{
	context->wait();
}
