#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Window.hpp"
#include "core/Context.hpp"
#include "core/SwapChain.hpp"
#include "core/RenderPass.hpp"
#include "core/GraphicsPipeline.hpp"
#include "core/Buffer.hpp"

struct Frame {
	uint32_t          image_index{};
	vk::CommandBuffer command{};
	vk::Semaphore     wait_semaphore{};
	vk::Semaphore     signal_semaphore{};
	vk::Fence         fence{};
};

struct Renderer {
	std::unique_ptr<Context>          context;
	std::unique_ptr<SwapChain>        swap_chain;
	std::unique_ptr<RenderPass>       render_pass;
	std::unique_ptr<GraphicsPipeline> graphics_pipeline;

	std::unique_ptr<Buffer> staging_buffer;
	std::unique_ptr<Buffer> vertex_buffer;
	std::unique_ptr<Buffer> index_buffer;

	Frame frame;

	Renderer(Window& window);
	~Renderer() = default;

	void render();
	void draw();
	void wait();
};
