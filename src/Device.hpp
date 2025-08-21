#pragma once

#include "Instance.hpp"
#include "Surface.hpp"

#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	operator bool() const;
};

class Device {
private:
	vk::PhysicalDevice physical_device{};
	vk::Device         logical_device{};
	vk::Queue          graphics_queue{};
	vk::Queue          present_queue{};

	Instance& instance;
	Surface&  surface;

	std::vector<std::string> device_extensions;

	QueueFamilyIndices queue_family_indices;

	void pickPhysicalDevice();
	void createLogicalDevice();
	void queryQueueFamilyIndices();

public:
	Device(Instance& instance, Surface& surface);
	~Device();

	vk::PhysicalDevice getPhysicalDevice() const;
	vk::Device         getLogicalDevice() const;
	vk::Queue          getGraphicsQueue() const;
	vk::Queue          getPresentQueue() const;
	QueueFamilyIndices getQueueFamilyIndices() const;
};
