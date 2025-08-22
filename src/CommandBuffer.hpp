#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandPool.hpp"

class CommandBuffer {
private:
	vk::CommandBuffer command_buffer;

	Context&          context;
	SwapChain&        swap_chain;
	RenderPass&       render_pass;
	GraphicsPipeline& graphics_pipeline;
	CommandPool&      command_pool;

public:
	CommandBuffer(Context& context, SwapChain& swap_chain, RenderPass& render_pass, GraphicsPipeline& graphics_pipeline, CommandPool& command_pool);
	~CommandBuffer();

	void record(uint32_t image_index);

	vk::CommandBuffer get() const;
};
