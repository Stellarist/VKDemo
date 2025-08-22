#include "RenderPass.hpp"

RenderPass::RenderPass(Context& context, SwapChain& swap_chain) :
    context(context),
    swap_chain(swap_chain)
{
	createRenderPass();
	createFrameBuffers();
}

RenderPass::~RenderPass()
{
	for (auto& framebuffer : framebuffers)
		context.getLogicalDevice().destroyFramebuffer(framebuffer);
	context.getLogicalDevice().destroyRenderPass(render_pass);
}

void RenderPass::createRenderPass()
{
	vk::AttachmentDescription color_attachment{};
	color_attachment.setFormat(swap_chain.getFormat().format)
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

	vk::RenderPassCreateInfo render_pass_info{};
	render_pass_info.setAttachments(color_attachment)
	    .setSubpasses(subpass)
	    .setDependencies(subpass_dependency);

	render_pass = context.getLogicalDevice().createRenderPass(render_pass_info);
}

void RenderPass::createFrameBuffers()
{
	framebuffers.resize(swap_chain.getImageCount());
	for (size_t i = 0; i < framebuffers.size(); i++) {
		vk::FramebufferCreateInfo create_info{};
		create_info.setRenderPass(render_pass)
		    .setAttachments(swap_chain.getImageViews()[i])
		    .setWidth(swap_chain.getExtent().width)
		    .setHeight(swap_chain.getExtent().height)
		    .setLayers(1);

		framebuffers[i] = context.getLogicalDevice().createFramebuffer(create_info);
	}
}

vk::RenderPass RenderPass::get() const
{
	return render_pass;
}

const std::vector<vk::Framebuffer>& RenderPass::getFrameBuffers() const
{
	return framebuffers;
}
