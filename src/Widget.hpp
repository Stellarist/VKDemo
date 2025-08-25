#pragma once

#include <functional>

#include <vulkan/vulkan.hpp>

#include "Window.hpp"
#include "core/Context.hpp"
#include "core/RenderPass.hpp"

struct WidgetInfo {
	vk::Instance       instance;
	vk::PhysicalDevice physical_device;
	vk::Device         logical_device;
	uint32_t           graphics_queue_family;
	vk::Queue          graphics_queue;
	vk::DescriptorPool descriptor_pool;
	vk::RenderPass     render_pass;
};

class Widget {
private:
	std::function<void()> callback;

public:
	Widget(Window& window, Context& context, RenderPass& render_pass);
	~Widget();

	void render(vk::CommandBuffer command_buffer);
	void newFrame();
	bool pollEvnet(const SDL_Event& event);
	void setCallback(std::function<void()> callback);
};
