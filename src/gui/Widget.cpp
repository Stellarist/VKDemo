#include "Widget.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

Widget::Widget(Window& window, Context& context, RenderPass& render_pass)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	if (!ImGui_ImplSDL3_InitForVulkan(window.get())) {
		ImGui::DestroyContext();
		throw std::runtime_error("Failed to initialize ImGui SDL3 backend");
	}

	ImGui_ImplVulkan_InitInfo init_info{
	    .Instance = context.getInstance(),
	    .PhysicalDevice = context.getPhysicalDevice(),
	    .Device = context.getLogicalDevice(),
	    .QueueFamily = context.getGraphicsQueueIndex(),
	    .Queue = context.getGraphicsQueue(),
	    // .DescriptorPool = context.getDescriptorManager().createPool(vk::DescriptorType::eCombinedImageSampler, 100),
	    .RenderPass = render_pass.get(),
	    .MinImageCount = 2,
	    .ImageCount = 3,
	};

	if (!ImGui_ImplVulkan_Init(&init_info))
		throw std::runtime_error("Failed to initialize ImGui Vulkan backend");
}

Widget::~Widget()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void Widget::render(vk::CommandBuffer command_buffer)
{
	callback();

	ImGui::Render();

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
}

void Widget::newFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

bool Widget::pollEvnet(const SDL_Event& event)
{
	return ImGui_ImplSDL3_ProcessEvent(&event);
}

void Widget::setCallback(std::function<void()> callback)
{
	this->callback = std::move(callback);
}
