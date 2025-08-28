#include "GraphicsPipeline.hpp"

#include "DescriptorManager.hpp"
#include "Shader.hpp"
#include "common/RenderData.hpp"

GraphicsPipeline::GraphicsPipeline(Context& c, RenderPass& r, const PipelineConfig& p) :
    context(&c),
    render_pass(&r),
    shader(c, SHADER_DIR "/default.spv"),
    config(p)
{
	shader.setStage(vk::ShaderStageFlagBits::eVertex, "vertexMain");
	shader.setStage(vk::ShaderStageFlagBits::eFragment, "fragmentMain");

	auto vbinding = Vertex::binding();
	auto vattributes = Vertex::attributes();
	auto tbinding = Transform::binding();
	descriptor_bindings.push_back(tbinding);

	auto layout = context->getDescriptorManager().createLayout(descriptor_bindings);

	config.vertex_input.setVertexBindingDescriptions(vbinding)
	    .setVertexAttributeDescriptions(vattributes);
	config.pipeline_layout.setSetLayouts(layout);

	pipeline_layout = context->getLogicalDevice().createPipelineLayout(config.pipeline_layout);

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

const std::vector<vk::DescriptorSetLayoutBinding>& GraphicsPipeline::getDescriptorBindings() const
{
	return descriptor_bindings;
}

const PipelineConfig& GraphicsPipeline::getConfig() const
{
	return config;
}

const Shader& GraphicsPipeline::getShader() const
{
	return shader;
}
