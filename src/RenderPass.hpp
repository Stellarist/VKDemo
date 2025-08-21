#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.hpp"
#include "SwapChain.hpp"

class RenderPass {
private:
	vk::RenderPass render_pass;

	std::vector<vk::Framebuffer> framebuffers;

	Device&    device;
	SwapChain& swap_chain;

	void createRenderPass();
	void createFrameBuffers();

public:
	RenderPass(Device& device, SwapChain& swap_chain);
	~RenderPass();

	vk::RenderPass get() const;

	const std::vector<vk::Framebuffer>& getFrameBuffers() const;
};
