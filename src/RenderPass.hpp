#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "SwapChain.hpp"

class RenderPass {
private:
	vk::RenderPass render_pass;

	std::vector<vk::Framebuffer> framebuffers;

	Context&   context;
	SwapChain& swap_chain;

	void createRenderPass();
	void createFrameBuffers();

public:
	RenderPass(Context& context, SwapChain& swap_chain);
	~RenderPass();

	vk::RenderPass get() const;

	const std::vector<vk::Framebuffer>& getFrameBuffers() const;
};
