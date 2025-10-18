#include "GraphicsPipeline.hpp"

#include "DescriptorManager.hpp"
#include "Shader.hpp"
#include "render/rhi/GpuVertex.hpp"
#include "render/rhi/GpuUniforms.hpp"

GraphicsPipeline::GraphicsPipeline(Context& c, RenderPass& r, const GraphicsPipelineConfig& p) :
    context(&c),
    render_pass(&r),
    shader(c, SHADER_DIR "/default.spv"),
    config(p)
{
	// move these to Shader
	shader.setStage(vk::ShaderStageFlagBits::eVertex, "vertexMain");
	shader.setStage(vk::ShaderStageFlagBits::eFragment, "fragmentMain");

	auto vbinding = GpuVertex::binding();
	auto vattributes = GpuVertex::attributes();

	descriptor_bindings.push_back(GpuTransform::binding(0));
	descriptor_bindings.push_back(Sampler::binding(1));

	vk::DescriptorSetLayoutBinding tex_binding{};
	tex_binding.setBinding(1)
	    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
	    .setStageFlags(vk::ShaderStageFlagBits::eFragment);

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

void GraphicsPipeline::create(const GraphicsPipelineConfig& config)
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

const GraphicsPipelineConfig& GraphicsPipeline::getConfig() const
{
	return config;
}

const Shader& GraphicsPipeline::getShader() const
{
	return shader;
}
