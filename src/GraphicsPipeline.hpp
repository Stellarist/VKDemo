#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "RenderPass.hpp"

class SwapChain;
class CommandBuffer;
class SyncObjects;

class GraphicsPipeline {
private:
	vk::Pipeline       pipeline;
	vk::PipelineLayout pipeline_layout;

	Context&    context;
	RenderPass& render_pass;

public:
	GraphicsPipeline(Context& context, RenderPass& render_pass);
	~GraphicsPipeline();

	vk::Pipeline       get() const;
	vk::PipelineLayout getLayout() const;

	void render(SwapChain* swap_chain, CommandBuffer* command_buffer, SyncObjects* sync_objects);
};
