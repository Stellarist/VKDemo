#pragma once

#include <vulkan/vulkan.hpp>

#include "Window.hpp"
#include "Context.hpp"

class SwapChain {
private:
	vk::SwapchainKHR                swap_chain;
	vk::Extent2D                    extent;
	vk::SurfaceFormatKHR            format;
	vk::PresentModeKHR              present;
	vk::SurfaceTransformFlagBitsKHR transform;

	uint32_t                   image_count;
	std::vector<vk::Image>     images;
	std::vector<vk::ImageView> image_views;

	Window&  window;
	Context& context;

	void createSwapChain();
	void createImageViews();
	void querySwapChainInfos(uint32_t width, uint32_t height);

public:
	SwapChain(Window& window, Context& context);
	~SwapChain();

	vk::SwapchainKHR                get() const;
	vk::Extent2D                    getExtent() const;
	vk::SurfaceFormatKHR            getFormat() const;
	vk::PresentModeKHR              getPresent() const;
	vk::SurfaceTransformFlagBitsKHR getTransform() const;

	uint32_t                          getImageCount() const;
	const std::vector<vk::Image>&     getImages() const;
	const std::vector<vk::ImageView>& getImageViews() const;
};
