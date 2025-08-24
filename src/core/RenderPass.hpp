#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "SwapChain.hpp"

class RenderPass {
private:
	vk::RenderPass render_pass;

	std::vector<vk::Framebuffer> framebuffers;

	Context*   context{};
	SwapChain* swap_chain{};

	void createForwardPass(vk::Format color_format);
	void createCustomPass(std::span<const vk::AttachmentDescription> attachments,
	                      std::span<const vk::SubpassDescription>    subpasses,
	                      std::span<const vk::SubpassDependency>     dependencies);

	void createFrameBuffers(std::span<const vk::ImageView> attachments, vk::Extent2D extent, uint32_t count);

public:
	RenderPass(Context& context, SwapChain& swap_chain);
	~RenderPass();

	void begin(vk::CommandBuffer command_buffer, uint32_t framebuffer_index, const vk::Extent2D& extent, const vk::ClearValue& color);
	void end(vk::CommandBuffer command_buffer);
	void next(vk::CommandBuffer command_buffer);

	vk::RenderPass get() const;
};
