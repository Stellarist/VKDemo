#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class SyncObjects {
private:
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence     in_flight_fence;

	Context* context;

public:
	SyncObjects(Context& context);
	~SyncObjects();

	vk::Semaphore getImageAvailableSemaphore();
	vk::Semaphore getRenderFinishedSemaphore();
	vk::Fence     getInFlightFence();
};
