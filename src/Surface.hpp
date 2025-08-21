#pragma once

#include <vulkan/vulkan.hpp>

#include "Window.hpp"
#include "Instance.hpp"

class Surface {
private:
	vk::SurfaceKHR surface{};

	Window&   window;
	Instance& instance;

public:
	Surface(Window& window, Instance& instance);
	~Surface();

	vk::SurfaceKHR& get();
};
