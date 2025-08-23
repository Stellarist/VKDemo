#include "GraphicsPipeline.hpp"

#include <print>

#include "Shader.hpp"
#include "SwapChain.hpp"
#include "SyncObjects.hpp"

GraphicsPipeline::GraphicsPipeline(Context& context, RenderPass& render_pass) :
    context(&context),
    render_pass(&render_pass),
    shader(context.getLogicalDevice(), SHADER_DIR "/default.spv")

{
	create();
}

GraphicsPipeline::~GraphicsPipeline()
{
	context->getLogicalDevice().destroyPipeline(pipeline);
	context->getLogicalDevice().destroyPipelineLayout(pipeline_layout);
}

void GraphicsPipeline::create()
{
	auto stages = shader.getStages();

	pipeline_layout = context->getLogicalDevice().createPipelineLayout(config.pipeline_layout);

	vk::GraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.setStages(stages)
	    .setPVertexInputState(&config.vertex_input)
	    .setPInputAssemblyState(&config.input_assembly)
	    .setPViewportState(&config.viewport)
	    .setPRasterizationState(&config.rasterizer)
	    .setPMultisampleState(&config.multisample)
	    .setPDepthStencilState(&config.depth_stencil)
	    .setPColorBlendState(&config.color_blend_state)
	    .setPDynamicState(&config.dynamic_state)
	    .setLayout(pipeline_layout)
	    .setRenderPass(render_pass->get())
	    .setSubpass(0);

	pipeline = context->getLogicalDevice().createGraphicsPipeline({}, pipeline_info).value;
}

void GraphicsPipeline::render(SwapChain* swap_chain, CommandBuffer* command_buffer, SyncObjects* sync_objects)
{
	auto image_available_semaphore = sync_objects->getImageAvailableSemaphore();
	auto render_finished_semaphore = sync_objects->getRenderFinishedSemaphore();
	auto in_flight_fence = sync_objects->getInFlightFence();

	if (context->getLogicalDevice().waitForFences(in_flight_fence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
		std::println("Failed to wait for fence");
	context->getLogicalDevice().resetFences(in_flight_fence);

	auto [result, image_index] = context->getLogicalDevice().acquireNextImageKHR(swap_chain->get(), std::numeric_limits<uint64_t>::max(), image_available_semaphore);
	if (result != vk::Result::eSuccess)
		std::println("Failed to acquire swap chain image: {}", vk::to_string(result));

	command_buffer->get().reset();
	command_buffer->record(image_index);

	std::array swap_chains = {swap_chain->get()};
	std::array command_buffers = {command_buffer->get()};

	vk::SubmitInfo         submit_info{};
	vk::PipelineStageFlags wait_stages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	submit_info.setCommandBuffers(command_buffers)
	    .setWaitSemaphoreCount(1)
	    .setPWaitDstStageMask(&wait_stages)
	    .setWaitSemaphores(image_available_semaphore)
	    .setSignalSemaphores(render_finished_semaphore);
	context->getGraphicsQueue().submit(submit_info, in_flight_fence);

	vk::PresentInfoKHR present_info{};
	present_info.setImageIndices(image_index)
	    .setSwapchains(swap_chains)
	    .setWaitSemaphores(render_finished_semaphore);
	if (context->getGraphicsQueue().presentKHR(present_info) != vk::Result::eSuccess)
		std::println("Failed to present swap chain image");
}

vk::Pipeline GraphicsPipeline::get() const
{
	return pipeline;
}

vk::PipelineLayout GraphicsPipeline::getLayout() const
{
	return pipeline_layout;
}
