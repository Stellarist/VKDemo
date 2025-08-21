#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Window.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "SyncObjects.hpp"

class Application {
private:
	Window           window;
	Instance         instance;
	Surface          surface;
	Device           device;
	SwapChain        swap_chain;
	RenderPass       render_pass;
	GraphicsPipeline graphics_pipeline;
	CommandPool      command_pool;
	CommandBuffer    command_buffer;
	SyncObjects      sync_objects;

public:
	Application();

	void run();
};
