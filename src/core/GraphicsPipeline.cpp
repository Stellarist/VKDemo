#include "GraphicsPipeline.hpp"

#include "Shader.hpp"
#include "Mesh.hpp"

GraphicsPipeline::GraphicsPipeline(Context& context, RenderPass& render_pass) :
    context(&context),
    render_pass(&render_pass),
    shader(context, SHADER_DIR "/default.spv")
{
	shader.setStage(vk::ShaderStageFlagBits::eVertex, "vertexMain");
	shader.setStage(vk::ShaderStageFlagBits::eFragment, "fragmentMain");

	auto binding = Vertex::binding();
	auto attributes = Vertex::attributes();

	PipelineConfig config{};
	config.vertex_input.setVertexBindingDescriptions(binding)
	    .setVertexAttributeDescriptions(attributes);
	config.pipeline_layout.setSetLayouts({});

	pipeline_layout = context.getLogicalDevice().createPipelineLayout(config.pipeline_layout);

	create(config);
}

GraphicsPipeline::GraphicsPipeline(Context& context, RenderPass& render_pass, const PipelineConfig& config) :
    context(&context),
    render_pass(&render_pass),
    shader(context, SHADER_DIR "/default.spv")
{
	shader.setStage(vk::ShaderStageFlagBits::eVertex, "vertexMain");
	shader.setStage(vk::ShaderStageFlagBits::eFragment, "fragmentMain");

	create(config);
}

GraphicsPipeline::~GraphicsPipeline()
{
	context->getLogicalDevice().destroyPipeline(pipeline);
	context->getLogicalDevice().destroyPipelineLayout(pipeline_layout);
}

void GraphicsPipeline::create(const PipelineConfig& config)
{
	auto stages = shader.getStages();

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

vk::Pipeline GraphicsPipeline::get() const
{
	return pipeline;
}

vk::PipelineLayout GraphicsPipeline::getLayout() const
{
	return pipeline_layout;
}

Shader& GraphicsPipeline::getShader()
{
	return shader;
}
