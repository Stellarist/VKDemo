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
#include "SyncObjects.hpp"

class Application {
private:
	Window           window;
	Context          context;
	SwapChain        swap_chain;
	RenderPass       render_pass;
	GraphicsPipeline graphics_pipeline;
	CommandManager   command_manager;
	SyncObjects      sync_objects;

public:
	Application();

	void run();
};
