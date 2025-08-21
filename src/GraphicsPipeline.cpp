#include "GraphicsPipeline.hpp"

#include <print>

#include "Shader.hpp"
#include "SwapChain.hpp"
#include "CommandBuffer.hpp"
#include "SyncObjects.hpp"

GraphicsPipeline::GraphicsPipeline(Device& device, RenderPass& render_pass) :
    device(device),
    render_pass(render_pass)
{
	Shader shader_module(SHADER_DIR "/main.spv", device);

	vk::PipelineShaderStageCreateInfo vertex_stage_info{};
	vertex_stage_info.setStage(vk::ShaderStageFlagBits::eVertex)
	    .setModule(shader_module.get())
	    .setPName("vertexMain");

	vk::PipelineShaderStageCreateInfo fragment_stage_info{};
	fragment_stage_info.setStage(vk::ShaderStageFlagBits::eFragment)
	    .setModule(shader_module.get())
	    .setPName("fragmentMain");

	std::array shader_stages = {vertex_stage_info, fragment_stage_info};

	vk::PipelineVertexInputStateCreateInfo vertex_input{};
	vertex_input.setVertexBindingDescriptionCount(0)
	    .setVertexAttributeDescriptionCount(0);

	vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
	input_assembly.setPrimitiveRestartEnable(vk::False)
	    .setTopology(vk::PrimitiveTopology::eTriangleList);

	vk::PipelineViewportStateCreateInfo viewport{};
	viewport.setViewportCount(1)
	    .setScissorCount(1);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.setDepthClampEnable(vk::False)
	    .setRasterizerDiscardEnable(vk::False)
	    .setPolygonMode(vk::PolygonMode::eFill)
	    .setLineWidth(1.0f)
	    .setCullMode(vk::CullModeFlagBits::eBack)
	    .setFrontFace(vk::FrontFace::eCounterClockwise)
	    .setDepthBiasEnable(vk::False);

	vk::PipelineMultisampleStateCreateInfo multisample{};
	multisample.setSampleShadingEnable(vk::False)
	    .setRasterizationSamples(vk::SampleCountFlagBits::e1);

	vk::PipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.setBlendEnable(vk::False)
	    .setColorWriteMask(vk::ColorComponentFlagBits::eR |
	                       vk::ColorComponentFlagBits::eG |
	                       vk::ColorComponentFlagBits::eB |
	                       vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo color_blend_state{};
	color_blend_state.setLogicOpEnable(vk::False)
	    .setLogicOp(vk::LogicOp::eCopy)
	    .setAttachments(color_blend_attachment);

	std::array dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	vk::PipelineDynamicStateCreateInfo dynamic_state{};
	dynamic_state.setDynamicStateCount(dynamic_states.size())
	    .setPDynamicStates(dynamic_states.data());

	vk::PipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.setSetLayoutCount(0)
	    .setPushConstantRangeCount(0);

	pipeline_layout = device.getLogicalDevice().createPipelineLayout(pipeline_layout_info);

	vk::GraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.setStageCount(shader_stages.size())
	    .setPStages(shader_stages.data())
	    .setPVertexInputState(&vertex_input)
	    .setPInputAssemblyState(&input_assembly)
	    .setPViewportState(&viewport)
	    .setPRasterizationState(&rasterizer)
	    .setPMultisampleState(&multisample)
	    .setPColorBlendState(&color_blend_state)
	    .setPDynamicState(&dynamic_state)
	    .setLayout(pipeline_layout)
	    .setRenderPass(render_pass.get())
	    .setSubpass(0);

	pipeline = device.getLogicalDevice().createGraphicsPipeline({}, pipeline_info).value;
}

GraphicsPipeline::~GraphicsPipeline()
{
	device.getLogicalDevice().destroyPipeline(pipeline);
	device.getLogicalDevice().destroyPipelineLayout(pipeline_layout);
}

void GraphicsPipeline::render(SwapChain* swap_chain, CommandBuffer* command_buffer, SyncObjects* sync_objects)
{
	auto image_available_semaphore = sync_objects->getImageAvailableSemaphore();
	auto render_finished_semaphore = sync_objects->getRenderFinishedSemaphore();
	auto in_flight_fence = sync_objects->getInFlightFence();

	if (device.getLogicalDevice().waitForFences(in_flight_fence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
		std::println("Failed to wait for fence");
	device.getLogicalDevice().resetFences(in_flight_fence);

	auto [result, image_index] = device.getLogicalDevice().acquireNextImageKHR(swap_chain->get(), std::numeric_limits<uint64_t>::max(), image_available_semaphore);
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
	device.getGraphicsQueue().submit(submit_info, in_flight_fence);

	vk::PresentInfoKHR present_info{};
	present_info.setImageIndices(image_index)
	    .setSwapchains(swap_chains)
	    .setWaitSemaphores(render_finished_semaphore);
	if (device.getGraphicsQueue().presentKHR(present_info) != vk::Result::eSuccess)
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
