#include "Renderer.hpp"

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "core/CommandManager.hpp"
#include "core/SyncManager.hpp"
#include "core/Mesh.hpp"

constexpr std::array<Vertex, 4> vertices = {
    Vertex{{-0.5, -0.5, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0}, {1.0, 0.0, 0.0, 1.0}},
    Vertex{{-0.5, 0.5, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}},
    Vertex{{0.5, 0.5, 0.0}, {0.0, 0.0, -1.0}, {1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}},
    Vertex{{0.5, -0.5, 0.0}, {0.0, 0.0, -1.0}, {1.0, 0.0}, {0.0, 1.0, 0.0, 1.0}},
};

constexpr std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

Renderer::Renderer(Window& window)
{
	context = std::make_unique<Context>(window);
	swap_chain = std::make_unique<SwapChain>(window, *context);
	render_pass = std::make_unique<RenderPass>(*context, *swap_chain);
	graphics_pipeline = std::make_unique<GraphicsPipeline>(*context, *render_pass);

	staging_buffer = std::make_unique<Buffer>(*context, sizeof(vertices),
	                                          vk::BufferUsageFlagBits::eTransferSrc,
	                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	staging_buffer->upload(static_cast<void*>(const_cast<Vertex*>(vertices.data())), sizeof(vertices));
	vertex_buffer = std::make_unique<Buffer>(*context, sizeof(vertices),
	                                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
	                                         vk::MemoryPropertyFlagBits::eDeviceLocal);
	vertex_buffer->copyFrom(staging_buffer->get(), sizeof(vertices));

	staging_buffer = std::make_unique<Buffer>(*context, sizeof(vertices),
	                                          vk::BufferUsageFlagBits::eTransferSrc,
	                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	staging_buffer->upload(static_cast<void*>(const_cast<uint32_t*>(indices.data())), sizeof(indices));
	index_buffer = std::make_unique<Buffer>(*context, sizeof(indices),
	                                        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
	                                        vk::MemoryPropertyFlagBits::eDeviceLocal);
	index_buffer->copyFrom(staging_buffer->get(), sizeof(indices));

	frame.command = context->getCommandManager().allocateBuffer();
	frame.wait_semaphore = context->getSyncManager().allocateSemaphore();
	frame.signal_semaphore = context->getSyncManager().allocateSemaphore();
	frame.fence = context->getSyncManager().allocateFence();
}

void Renderer::render()
{
	auto command_manager = &context->getCommandManager();
	auto sync_manager = &context->getSyncManager();
	auto chain = swap_chain->get();
	auto stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	sync_manager->waitForFence(frame.fence);
	sync_manager->resetFence(frame.fence);

	frame.image_index = swap_chain->acquireNextImage(frame.wait_semaphore, nullptr);
	frame.command.reset();

	command_manager->begin(frame.command);
	render_pass->begin(frame.command, frame.image_index, swap_chain->getExtent(), {{0.0f, 0.0f, 0.0f, 1.0f}});

	frame.command.setScissor(0,
	                         vk::Rect2D{}
	                             .setOffset({0, 0})
	                             .setExtent(swap_chain->getExtent()));
	frame.command.setViewport(0,
	                          vk::Viewport{}
	                              .setX(0.0f)
	                              .setY(0.0f)
	                              .setWidth(static_cast<float>(swap_chain->getExtent().width))
	                              .setHeight(static_cast<float>(swap_chain->getExtent().height))
	                              .setMinDepth(0.0f)
	                              .setMaxDepth(1.0f));

	draw();

	render_pass->end(frame.command);
	command_manager->end(frame.command);

	context->submit(frame.command, {&frame.wait_semaphore, 1}, {&frame.signal_semaphore, 1}, {&stage, 1}, frame.fence);
	context->present({&frame.image_index, 1}, {&chain, 1}, {&frame.signal_semaphore, 1});

	frame.fence = sync_manager->nextFence(frame.fence);
}

void Renderer::draw()
{
	frame.command.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline->get());
	frame.command.bindVertexBuffers(0, vertex_buffer->get(), {0});
	frame.command.bindIndexBuffer(index_buffer->get(), 0, vk::IndexType::eUint32);
	frame.command.drawIndexed(indices.size(), 1, 0, 0, 0);
}

void Renderer::wait()
{
	context->getLogicalDevice().waitIdle();
}
