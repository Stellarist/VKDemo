#include "Renderer.hpp"

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/DescriptorManager.hpp"
#include "graphics/CommandManager.hpp"
#include "graphics/SyncManager.hpp"
#include "graphics/Buffer.hpp"
#include "render/rhi/GpuVertex.hpp"
#include "render/rhi/GpuUniforms.hpp"

std::array<GpuVertex, 4> vertices = {
    GpuVertex{{-0.5, -0.5, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0}, {1.0, 0.0, 0.0, 1.0}},
    GpuVertex{{-0.5, 0.5, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}},
    GpuVertex{{0.5, 0.5, 0.0}, {0.0, 0.0, -1.0}, {1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}},
    GpuVertex{{0.5, -0.5, 0.0}, {0.0, 0.0, -1.0}, {1.0, 0.0}, {0.0, 1.0, 0.0, 1.0}},
};

std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

GpuTransform createTransform()
{
	// Vulkan使用右手坐标系，Y轴向下，Z范围0到1
	auto proj = glm::perspective(glm::radians(45.0f), 2560.0f / 1440.0f, 0.1f, 100.0f);
	proj[1][1] *= -1.0f;        // 翻转Y轴以适配Vulkan

	return GpuTransform{
	    .model = glm::mat4(1.0f),
	    .view = glm::lookAt(glm::vec3(0.0f, 6.0f, 12.0f), glm::vec3(0.0f, 0.75f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	    .projection = proj,
	};
}

GpuTransform transform = createTransform();

Renderer::Renderer(Window& window)
{
	context = std::make_unique<Context>(window);
	swap_chain = std::make_unique<SwapChain>(window, *context);
	render_pass = std::make_unique<RenderPass>(*context, *swap_chain);
	graphics_pipeline = std::make_unique<GraphicsPipeline>(*context, *render_pass);

	vertex_buffer = Buffer::createFrom(*context, vk::BufferUsageFlagBits::eVertexBuffer, vertices.data(), sizeof(vertices));
	index_buffer = Buffer::createFrom(*context, vk::BufferUsageFlagBits::eIndexBuffer, indices.data(), sizeof(indices));
	uniform_buffer = Buffer::createFrom(*context, vk::BufferUsageFlagBits::eUniformBuffer, &transform, sizeof(GpuTransform));
	image = std::make_unique<Image>(*context, ASSETS_DIR "/background.jpeg");
	sampler = std::make_unique<Sampler>(*context);
	image->setSampler(*sampler);

	frame.command = context->getCommandManager().allocateBuffer();
	frame.wait_semaphore = context->getSyncManager().allocateSemaphore();
	frame.signal_semaphore = context->getSyncManager().allocateSemaphore();
	frame.fence = context->getSyncManager().allocateFence();
	context->getSyncManager().allocateFence();

	frame.pool = context->getDescriptorManager().createPool(vk::DescriptorType::eUniformBuffer, 1);
	frame.set = context->getDescriptorManager().allocateSet(frame.pool, graphics_pipeline->getDescriptorBindings());

	context->getDescriptorManager().updateSet(frame.set, 0, vk::DescriptorType::eUniformBuffer, uniform_buffer.get());
	context->getDescriptorManager().updateSet(frame.set, 1, vk::DescriptorType::eCombinedImageSampler, image.get());
}

void Renderer::begin()
{
	auto* command_manager = &context->getCommandManager();
	auto* sync_manager = &context->getSyncManager();

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
}

void Renderer::end()
{
	auto chain = swap_chain->get();
	auto stage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	render_pass->end(frame.command);
	context->getCommandManager().end(frame.command);

	context->submit(frame.command, {&frame.wait_semaphore, 1}, {&frame.signal_semaphore, 1}, {&stage, 1}, frame.fence);
	context->present({&frame.image_index, 1}, {&chain, 1}, {&frame.signal_semaphore, 1});

	frame.fence = context->getSyncManager().nextFence(frame.fence);
}

void Renderer::setScene(const Scene& scene)
{
	render_scene = std::make_unique<RenderScene>(*context, scene);
}

void Renderer::draw()
{
	frame.command.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline->get());
	frame.command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphics_pipeline->getLayout(), 0, frame.set, {});

	// 如果有场景，绘制场景中的所有网格
	if (render_scene)
		render_scene->draw(frame.command);
	else {
		frame.command.bindVertexBuffers(0, vertex_buffer->get(), {0});
		frame.command.bindIndexBuffer(index_buffer->get(), 0, vk::IndexType::eUint32);
		frame.command.drawIndexed(indices.size(), 1, 0, 0, 0);
	}
}

void Renderer::wait()
{
	context->getLogicalDevice().waitIdle();
}
