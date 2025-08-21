#include "Surface.hpp"

#include <print>

Surface::Surface(Window& window, Instance& instance) :
    window(window),
    instance(instance)
{
	VkSurfaceKHR csurface{};
	if (!SDL_Vulkan_CreateSurface(window.get(), instance.get(), nullptr, &csurface)) {
		std::println("Failed to create Vulkan surface: {}", SDL_GetError());
		throw std::runtime_error("Vulkan surface creation failed");
	}

	surface = std::move(csurface);
}

Surface::~Surface()
{
	instance.get().destroySurfaceKHR(surface);
}

vk::SurfaceKHR& Surface::get()
{
	return surface;
}
