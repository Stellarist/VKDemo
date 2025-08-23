#include "RenderPass.hpp"

RenderPass::RenderPass(Context& context, SwapChain& swap_chain) :
    context(&context),
    swap_chain(&swap_chain)
{
	createForwardPass(swap_chain.getSurfaceFormat().format);
	createFrameBuffers(swap_chain.getImageViews(), swap_chain.getExtent(), swap_chain.getImageCount());
}

RenderPass::~RenderPass()
{
	for (auto& framebuffer : framebuffers)
		context->getLogicalDevice().destroyFramebuffer(framebuffer);
	context->getLogicalDevice().destroyRenderPass(render_pass);
}

void RenderPass::createForwardPass(vk::Format color_format)
{
	vk::AttachmentDescription color_attachment{};
	color_attachment.setFormat(color_format)
	    .setSamples(vk::SampleCountFlagBits::e1)
	    .setLoadOp(vk::AttachmentLoadOp::eClear)
	    .setStoreOp(vk::AttachmentStoreOp::eStore)
	    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
	    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
	    .setInitialLayout(vk::ImageLayout::eUndefined)
	    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference color_attachment_ref{};
	color_attachment_ref.setAttachment(0)
	    .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass{};
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
	    .setColorAttachments(color_attachment_ref);

	vk::SubpassDependency subpass_dependency{};
	subpass_dependency.setSrcSubpass(vk::SubpassExternal)
	    .setDstSubpass(0)
	    .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
	    .setSrcAccessMask(vk::AccessFlagBits::eNone)
	    .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
	    .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

	createCustomPass({&color_attachment, 1}, {&subpass, 1}, {&subpass_dependency, 1});
}

void RenderPass::createCustomPass(std::span<const vk::AttachmentDescription> attachments,
                                  std::span<const vk::SubpassDescription>    subpasses,
                                  std::span<const vk::SubpassDependency>     dependencies)
{
	vk::RenderPassCreateInfo render_pass_info{};
	render_pass_info.setAttachments(attachments)
	    .setSubpasses(subpasses)
	    .setDependencies(dependencies);

	render_pass = context->getLogicalDevice().createRenderPass(render_pass_info);
}

void RenderPass::createFrameBuffers(std::span<const vk::ImageView> attachments, vk::Extent2D extent, uint32_t count)
{
	framebuffers.resize(count);
	for (size_t i = 0; i < count; i++) {
		vk::FramebufferCreateInfo create_info{};
		create_info.setRenderPass(render_pass)
		    .setAttachments(attachments[i])
		    .setWidth(extent.width)
		    .setHeight(extent.height)
		    .setLayers(1);

		framebuffers[i] = context->getLogicalDevice().createFramebuffer(create_info);
	}
}

void RenderPass::begin(vk::CommandBuffer command_buffer, uint32_t framebuffer_index, vk::Extent2D extent, std::span<const vk::ClearValue> clear_values)
{
	vk::RenderPassBeginInfo begin_info{};
	begin_info.setRenderPass(render_pass)
	    .setFramebuffer(framebuffers[framebuffer_index])
	    .setRenderArea({{0, 0}, extent})
	    .setClearValues(clear_values);

	command_buffer.beginRenderPass(begin_info, vk::SubpassContents::eInline);
}

void RenderPass::end(vk::CommandBuffer command_buffer)
{
	command_buffer.endRenderPass();
}

void RenderPass::next(vk::CommandBuffer command_buffer)
{
	command_buffer.nextSubpass(vk::SubpassContents::eInline);
}

vk::RenderPass RenderPass::get() const
{
	return render_pass;
}
