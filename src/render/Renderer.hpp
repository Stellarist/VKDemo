#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "gui/Window.hpp"
#include "graphics/Context.hpp"
#include "graphics/SwapChain.hpp"
#include "graphics/RenderPass.hpp"
#include "graphics/GraphicsPipeline.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Image.hpp"
#include "graphics/Sampler.hpp"
#include "scene/Scene.hpp"
#include "RenderScene.hpp"

struct Frame {
	uint32_t           image_index{};
	vk::CommandBuffer  command{};
	vk::Semaphore      wait_semaphore{};
	vk::Semaphore      signal_semaphore{};
	vk::Fence          fence{};
	vk::DescriptorPool pool{};
	vk::DescriptorSet  set{};
};

struct Renderer {
	std::unique_ptr<Context>          context;
	std::unique_ptr<SwapChain>        swap_chain;
	std::unique_ptr<RenderPass>       render_pass;
	std::unique_ptr<GraphicsPipeline> graphics_pipeline;

	std::unique_ptr<Buffer>  vertex_buffer;
	std::unique_ptr<Buffer>  index_buffer;
	std::unique_ptr<Buffer>  uniform_buffer;
	std::unique_ptr<Image>   image;
	std::unique_ptr<Sampler> sampler;

	std::unique_ptr<RenderScene> render_scene;

	Frame frame;

	Renderer(Window& window);
	~Renderer() = default;

	void setScene(const Scene& scene);
	void begin();
	void end();
	void wait();
	void draw();
};
