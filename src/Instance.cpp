#include "Instance.hpp"

#include <SDL3/SDL_vulkan.h>

Instance::Instance()
{
	std::array layers = {"VK_LAYER_KHRONOS_validation"};
	auto       extensions = getExtensions();

	vk::ApplicationInfo app_info{};
	app_info.setApiVersion(VK_API_VERSION_1_4);

	vk::InstanceCreateInfo create_info{};
	create_info.setPApplicationInfo(&app_info)
	    .setEnabledLayerCount(layers.size())
	    .setPEnabledLayerNames(layers)
	    .setEnabledExtensionCount(extensions.size())
	    .setPEnabledExtensionNames(extensions);

	instance = vk::createInstance(create_info);
}

Instance::~Instance()
{
	instance.destroy();
}

vk::Instance Instance::get() const
{
	return instance;
}

std::vector<const char*> Instance::getExtensions()
{
	unsigned int count = 0;

	auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
	if (!extensions)
		throw std::runtime_error("Failed to get the number of required Vulkan extensions from SDL");

	return {extensions, extensions + count};
}
