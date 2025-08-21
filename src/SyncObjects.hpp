#pragma once

#include <vulkan/vulkan.hpp>

#include "Device.hpp"

class SyncObjects {
private:
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence     in_flight_fence;

	Device& device;

public:
	SyncObjects(Device& device);
	~SyncObjects();

	vk::Semaphore getImageAvailableSemaphore();
	vk::Semaphore getRenderFinishedSemaphore();
	vk::Fence     getInFlightFence();
};
