#include "SyncObjects.hpp"

SyncObjects::SyncObjects(Device& device) :
    device(device)
{
	vk::SemaphoreCreateInfo semaphore_info{};

	vk::FenceCreateInfo fence_info{};
	fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

	image_available_semaphore = device.getLogicalDevice().createSemaphore(semaphore_info);
	render_finished_semaphore = device.getLogicalDevice().createSemaphore(semaphore_info);
	in_flight_fence = device.getLogicalDevice().createFence(fence_info);
}

SyncObjects::~SyncObjects()
{
	device.getLogicalDevice().destroyFence(in_flight_fence);
	device.getLogicalDevice().destroySemaphore(render_finished_semaphore);
	device.getLogicalDevice().destroySemaphore(image_available_semaphore);
}

vk::Semaphore SyncObjects::getImageAvailableSemaphore()
{
	return image_available_semaphore;
}

vk::Semaphore SyncObjects::getRenderFinishedSemaphore()
{
	return render_finished_semaphore;
}

vk::Fence SyncObjects::getInFlightFence()
{
	return in_flight_fence;
}
