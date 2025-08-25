#include <set>
#include <print>

#include "CommandManager.hpp"
#include "SyncManager.hpp"

Context::Context(Window& window) :
    window(&window)
{
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();

	command_manager = std::make_unique<CommandManager>(*this);
	sync_manager = std::make_unique<SyncManager>(*this);
}

Context::~Context()
{
	sync_manager.reset();
	command_manager.reset();
	logical_device.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();
}

void Context::createInstance()
{
	uint32_t   count = 0;
	std::array layers = {"VK_LAYER_KHRONOS_validation"};

	auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);

	vk::ApplicationInfo app_info{};
	app_info.setApiVersion(VK_API_VERSION_1_4);

	vk::InstanceCreateInfo create_info{};
	create_info.setPApplicationInfo(&app_info)
	    .setEnabledLayerCount(layers.size())
	    .setPEnabledLayerNames(layers)
	    .setEnabledExtensionCount(count)
	    .setPpEnabledExtensionNames(extensions);

	instance = vk::createInstance(create_info);
}

void Context::createSurface()
{
	VkSurfaceKHR csurface{};
	if (!SDL_Vulkan_CreateSurface(window->get(), instance, nullptr, &csurface)) {
		std::println("Failed to create Vulkan surface: {}", SDL_GetError());
		throw std::runtime_error("Vulkan surface creation failed");
	}

	surface = std::move(csurface);
}

void Context::pickPhysicalDevice()
{
	auto devices = instance.enumeratePhysicalDevices();
	for (const auto& context : devices) {
		vk::PhysicalDeviceProperties properties = context.getProperties();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			physical_device = context;
			return;
		}
	}

	physical_device = devices.front();
}

void Context::createLogicalDevice()
{
	queue_family_indices = queryQueueFamilyIndices();

	std::array layers = {"VK_LAYER_KHRONOS_validation"};
	std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos{};

	std::set<uint32_t> unique_queue_families = {
	    queue_family_indices.graphics_family.value(),
	    queue_family_indices.present_family.value(),
	};

	float queue_priority = 1.0f;
	for (auto family : unique_queue_families) {
		vk::DeviceQueueCreateInfo queue_create_info{};
		queue_create_info.setPQueuePriorities(&queue_priority)
		    .setQueueCount(1)
		    .setQueueFamilyIndex(family);
		queue_create_infos.push_back(std::move(queue_create_info));
	}

	vk::DeviceCreateInfo create_info{};
	create_info.setQueueCreateInfos(queue_create_infos)
	    .setEnabledLayerCount(layers.size())
	    .setPEnabledLayerNames(layers)
	    .setEnabledExtensionCount(extensions.size())
	    .setPEnabledExtensionNames(extensions);

	logical_device = physical_device.createDevice(create_info);

	graphics_queue = logical_device.getQueue(queue_family_indices.graphics_family.value(), 0);
	present_queue = logical_device.getQueue(queue_family_indices.present_family.value(), 0);
}

QueueFamilyIndices Context::queryQueueFamilyIndices() const
{
	QueueFamilyIndices queue_family_indices;

	auto properties = physical_device.getQueueFamilyProperties();
	for (int i = 0; i < properties.size(); i++) {
		const auto& property = properties[i];

		if (property.queueFlags & vk::QueueFlagBits::eGraphics)
			queue_family_indices.graphics_family = i;

		if (physical_device.getSurfaceSupportKHR(i, surface))
			queue_family_indices.present_family = i;

		if (queue_family_indices)
			break;
	}

	return queue_family_indices;
}

void Context::execute(std::function<void(vk::CommandBuffer)> func)
{
	auto command_buffer = command_manager->getBuffer(command_manager->allocateBuffer());
	command_manager->begin(command_buffer);
	func(command_buffer);
	command_manager->end(command_buffer);
	command_manager->submit(command_buffer);
	wait();
	command_manager->freeBuffer(command_buffer);
}

void Context::wait()
{
	logical_device.waitIdle();
}

vk::Instance Context::getInstance() const
{
	return instance;
}

vk::SurfaceKHR Context::getSurface() const
{
	return surface;
}

vk::PhysicalDevice Context::getPhysicalDevice() const
{
	return physical_device;
}

vk::Device Context::getLogicalDevice() const
{
	return logical_device;
}

vk::Queue Context::getGraphicsQueue() const
{
	return graphics_queue;
}

vk::Queue Context::getPresentQueue() const
{
	return present_queue;
}

CommandManager& Context::getCommandManager() const
{
	return *command_manager;
}

SyncManager& Context::getSyncManager() const
{
	return *sync_manager;
}

uint32_t Context::getGraphicsQueueIndex() const
{
	return queue_family_indices.graphics_family.value();
}

uint32_t Context::getPresentQueueIndex() const
{
	return queue_family_indices.present_family.value();
}

QueueFamilyIndices::operator bool() const
{
	return graphics_family.has_value() && present_family.has_value();
}
