#include "Device.hpp"

#include <set>

Device::Device(Instance& instance, Surface& surface) :
    instance(instance),
    surface(surface)
{
	pickPhysicalDevice();
	createLogicalDevice();
}

Device::~Device()
{
	logical_device.destroy();
}

void Device::pickPhysicalDevice()
{
	auto devices = instance.get().enumeratePhysicalDevices();
	for (const auto& device : devices) {
		vk::PhysicalDeviceProperties properties = device.getProperties();
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			physical_device = device;
			return;
		}
	}

	physical_device = devices.front();
}

void Device::createLogicalDevice()
{
	queryQueueFamilyIndices();

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

void Device::queryQueueFamilyIndices()
{
	auto properties = physical_device.getQueueFamilyProperties();
	for (int i = 0; i < properties.size(); i++) {
		const auto& property = properties[i];

		if (property.queueFlags & vk::QueueFlagBits::eGraphics)
			queue_family_indices.graphics_family = i;

		if (physical_device.getSurfaceSupportKHR(i, surface.get()))
			queue_family_indices.present_family = i;

		if (queue_family_indices)
			break;
	}
}

vk::PhysicalDevice Device::getPhysicalDevice() const
{
	return physical_device;
}

vk::Device Device::getLogicalDevice() const
{
	return logical_device;
}

vk::Queue Device::getGraphicsQueue() const
{
	return graphics_queue;
}

vk::Queue Device::getPresentQueue() const
{
	return present_queue;
}

QueueFamilyIndices Device::getQueueFamilyIndices() const
{
	return queue_family_indices;
}

QueueFamilyIndices::operator bool() const
{
	return graphics_family.has_value() && present_family.has_value();
}
