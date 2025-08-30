#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

struct MemoryInfo {
	uint32_t size;
	uint32_t index;
};

class Buffer {
private:
	vk::Buffer       buffer;
	vk::DeviceSize   size;
	vk::DeviceMemory memory;

	vk::BufferUsageFlagBits usage;

	void* data{};

	Context* context{};

	MemoryInfo queryMemoryInfo(vk::MemoryPropertyFlags properties) const;

public:
	Buffer(Context& context, uint32_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	~Buffer();

	void create(vk::BufferUsageFlags usage, size_t size);
	void allocate(vk::MemoryPropertyFlags properties);
	void map(size_t map_size, size_t map_offset = {});
	void unmap();
	void bind(size_t bind_offset = 0);
	void copyTo(vk::Buffer dst, size_t size, size_t src_offset = 0, size_t dst_offset = 0);
	void copyFrom(vk::Buffer src, size_t size, size_t src_offset = 0, size_t dst_offset = 0);
	void upload(void* src, size_t src_size, size_t dst_offset = 0);

	static std::unique_ptr<Buffer> createAndUpload(Context& context, vk::BufferUsageFlagBits Usage, void* src, size_t size);

	vk::Buffer     get() const;
	vk::DeviceSize getSize() const;
};
