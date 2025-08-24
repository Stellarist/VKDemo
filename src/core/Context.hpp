#pragma once

#include <vulkan/vulkan.hpp>

#include "Window.hpp"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	operator bool() const;
};

class Context {
private:
	vk::Instance       instance;
	vk::SurfaceKHR     surface;
	vk::PhysicalDevice physical_device;
	vk::Device         logical_device;
	vk::Queue          graphics_queue;
	vk::Queue          present_queue;

	Window* window{};

	QueueFamilyIndices queue_family_indices;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();

	QueueFamilyIndices queryQueueFamilyIndices() const;

public:
	Context(Window& window);
	~Context();

	vk::Instance       getInstance() const;
	vk::SurfaceKHR     getSurface() const;
	vk::PhysicalDevice getPhysicalDevice() const;
	vk::Device         getLogicalDevice() const;
	vk::Queue          getGraphicsQueue() const;
	vk::Queue          getPresentQueue() const;
	QueueFamilyIndices getQueueFamilyIndices() const;
};
