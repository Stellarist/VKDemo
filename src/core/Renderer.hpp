#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Window.hpp"
#include "Context.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"

#include "Buffer.hpp"

struct Frame {
	uint32_t image_index{};
	uint32_t wait_semaphore_index{};
	uint32_t signal_semaphore_index{};
	uint32_t fence_index{};
	uint32_t command_buffer_index{};
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
