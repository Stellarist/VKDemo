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

class Renderer {
private:
	Window           window;
	Context          context;
	SwapChain        swap_chain;
	RenderPass       render_pass;
	GraphicsPipeline graphics_pipeline;
	CommandManager   command_manager;
	SyncManager      sync_manager;

	uint32_t image_index{};
	uint32_t command_buffer_index{};
	uint32_t wait_semaphore_index{};
	uint32_t signal_semaphore_index{};
	uint32_t fence_index{};

public:
	Renderer();

	void begin();
	void end();
	void render();

	// TODO: move window and other members to a separate class
	void run();
};
