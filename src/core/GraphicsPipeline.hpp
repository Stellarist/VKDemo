#pragma once

#include <array>

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"

struct PipelineConfig {
	vk::PipelineVertexInputStateCreateInfo vertex_input{};

	vk::PipelineInputAssemblyStateCreateInfo input_assembly{
	    {},
	    vk::PrimitiveTopology::eTriangleList,
	    vk::False,
	};

	vk::PipelineTessellationStateCreateInfo tessellation{};

	vk::PipelineViewportStateCreateInfo viewport{
	    {},
	    1,
	    {},
	    1,
	    {},
	};

	vk::PipelineRasterizationStateCreateInfo rasterizer{
	    {},
	    vk::False,
	    vk::False,
	    vk::PolygonMode::eFill,
	    vk::CullModeFlagBits::eBack,
	    vk::FrontFace::eCounterClockwise,
	    vk::False,
	    {},
	    {},
	    {},
	    1.0f,
	};

	vk::PipelineMultisampleStateCreateInfo multisample{
	    {},
	    vk::SampleCountFlagBits::e1,
	    vk::False,
	};

	vk::PipelineDepthStencilStateCreateInfo depth_stencil{};

	vk::PipelineColorBlendAttachmentState color_blend_attachment{
	    vk::False,
	    {},
	    {},
	    {},
	    {},
	    {},
	    {},
	    vk::ColorComponentFlagBits::eR |
	        vk::ColorComponentFlagBits::eG |
	        vk::ColorComponentFlagBits::eB |
	        vk::ColorComponentFlagBits::eA,
	};

	vk::PipelineColorBlendStateCreateInfo color_blend_state{
	    {},
	    vk::False,
	    vk::LogicOp::eCopy,
	    1,
	    &color_blend_attachment,
	};

	std::array<vk::DynamicState, 2> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	vk::PipelineDynamicStateCreateInfo dynamic_state{
	    {},
	    static_cast<uint32_t>(dynamic_states.size()),
	    dynamic_states.data(),
	};

	vk::PipelineLayoutCreateInfo pipeline_layout{};
};

class GraphicsPipeline {
private:
	vk::Pipeline       pipeline;
	vk::PipelineLayout pipeline_layout;

	Shader shader;

	Context*    context{};
	RenderPass* render_pass{};

public:
	GraphicsPipeline(Context& context, RenderPass& render_pass);
	GraphicsPipeline(Context& context, RenderPass& render_pass, const PipelineConfig& config);
	~GraphicsPipeline();

	void create(const PipelineConfig& config);
	void bind(vk::CommandBuffer command_buffer);

	vk::Pipeline       get() const;
	vk::PipelineLayout getLayout() const;

	Shader& getShader();

	void create(Shader& shader);
};
