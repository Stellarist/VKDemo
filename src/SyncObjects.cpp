#include "SyncObjects.hpp"

SyncObjects::SyncObjects(Context& c) :
    context(&c)
{
	vk::SemaphoreCreateInfo semaphore_info{};

	vk::FenceCreateInfo fence_info{};
	fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

	image_available_semaphore = context->getLogicalDevice().createSemaphore(semaphore_info);
	render_finished_semaphore = context->getLogicalDevice().createSemaphore(semaphore_info);
	in_flight_fence = context->getLogicalDevice().createFence(fence_info);
}

SyncObjects::~SyncObjects()
{
	context->getLogicalDevice().destroyFence(in_flight_fence);
	context->getLogicalDevice().destroySemaphore(render_finished_semaphore);
	context->getLogicalDevice().destroySemaphore(image_available_semaphore);
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
