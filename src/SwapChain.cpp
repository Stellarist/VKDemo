#include "SwapChain.hpp"

SwapChain::SwapChain(Window& window, Device& device, Surface& surface) :
    window(window),
    device(device),
    surface(surface)
{
	createSwapChain();
	createImageViews();
}

SwapChain::~SwapChain()
{
	for (auto& view : image_views)
		device.getLogicalDevice().destroyImageView(view);
	device.getLogicalDevice().destroySwapchainKHR(swap_chain);
}

void SwapChain::createSwapChain()
{
	querySwapChainInfos(window.getWidth(), window.getHeight());

	vk::SwapchainCreateInfoKHR create_info{};
	create_info.setClipped(true)
	    .setImageArrayLayers(1)
	    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
	    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
	    .setSurface(surface.get())
	    .setImageColorSpace(format.colorSpace)
	    .setImageFormat(format.format)
	    .setImageExtent(extent)
	    .setMinImageCount(image_count)
	    .setPreTransform(transform)
	    .setClipped(vk::True)
	    .setPresentMode(present);

	auto [graphics_family, present_family] = device.getQueueFamilyIndices();
	if (present_family.value() == graphics_family.value())
		create_info.setQueueFamilyIndices(present_family.value())
		    .setImageSharingMode(vk::SharingMode::eExclusive);
	else {
		std::array queue_families = {graphics_family.value(), present_family.value()};
		create_info.setQueueFamilyIndices(queue_families)
		    .setImageSharingMode(vk::SharingMode::eConcurrent);
	}

	swap_chain = device.getLogicalDevice().createSwapchainKHR(create_info);

	images = device.getLogicalDevice().getSwapchainImagesKHR(swap_chain);
	image_views.resize(images.size());
}

void SwapChain::createImageViews()
{
	for (int i = 0; i < image_views.size(); i++) {
		vk::ImageSubresourceRange range{};
		range.setBaseMipLevel(0)
		    .setLevelCount(1)
		    .setBaseArrayLayer(0)
		    .setLayerCount(1)
		    .setAspectMask(vk::ImageAspectFlagBits::eColor);

		vk::ComponentMapping mapping{};
		mapping.setA(vk::ComponentSwizzle::eIdentity)
		    .setR(vk::ComponentSwizzle::eIdentity)
		    .setG(vk::ComponentSwizzle::eIdentity)
		    .setB(vk::ComponentSwizzle::eIdentity);

		vk::ImageViewCreateInfo create_info{};
		create_info.setImage(images[i])
		    .setViewType(vk::ImageViewType::e2D)
		    .setFormat(format.format)
		    .setSubresourceRange(range)
		    .setComponents(mapping);

		image_views[i] = device.getLogicalDevice().createImageView(create_info);
	}
}

void SwapChain::querySwapChainInfos(uint32_t width, uint32_t height)
{
	auto formats = device.getPhysicalDevice().getSurfaceFormatsKHR(surface.get());
	auto fit = std::find_if(formats.begin(), formats.end(), [](const vk::SurfaceFormatKHR& format) {
		return format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
	});
	this->format = (fit != formats.end()) ? *fit : formats.front();

	auto presents = device.getPhysicalDevice().getSurfacePresentModesKHR(surface.get());
	auto pit = std::find_if(presents.begin(), presents.end(), [](const vk::PresentModeKHR& present) {
		return present == vk::PresentModeKHR::eMailbox;
	});
	this->present = (pit != presents.end()) ? *pit : vk::PresentModeKHR::eFifo;

	auto capabilities = device.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface.get());
	image_count = std::clamp<uint32_t>(capabilities.minImageCount + 1, capabilities.minImageCount, capabilities.maxImageCount);
	extent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	transform = capabilities.currentTransform;
}

vk::SwapchainKHR SwapChain::get() const
{
	return swap_chain;
}

vk::Extent2D SwapChain::getExtent() const
{
	return extent;
}

vk::SurfaceFormatKHR SwapChain::getFormat() const
{
	return format;
}

vk::PresentModeKHR SwapChain::getPresent() const
{
	return present;
}

vk::SurfaceTransformFlagBitsKHR SwapChain::getTransform() const
{
	return transform;
}

uint32_t SwapChain::getImageCount() const
{
	return image_count;
}

const std::vector<vk::Image>& SwapChain::getImages() const
{
	return images;
}

const std::vector<vk::ImageView>& SwapChain::getImageViews() const
{
	return image_views;
}
