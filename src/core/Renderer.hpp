#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Window.hpp"
#include "Context.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandManager.hpp"
#include "SyncManager.hpp"
#include "Widget.hpp"

#include "Buffer.hpp"

struct Frame {
	uint32_t image_index{};
	uint32_t wait_semaphore_index{};
	uint32_t signal_semaphore_index{};
	uint32_t fence_index{};
	uint32_t command_buffer_index{};
};

class Renderer {
private:
	Context          context;
	SwapChain        swap_chain;
	RenderPass       render_pass;
	GraphicsPipeline graphics_pipeline;
	CommandManager   command_manager;
	SyncManager      sync_manager;
	Widget           widget;

	Frame frame;

	Buffer vertex_buffer;

public:
	Renderer(Window& window);

	void begin();
	void end();
	void render();
	void wait();
};
