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

public:
	Buffer(Context& context, uint32_t size, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags properties);
	~Buffer();

	void create(vk::BufferUsageFlagBits usage, size_t size);
	void allocate(vk::MemoryPropertyFlags properties);
	void map(size_t map_size, size_t map_offset);
	void unmap();
	void bind(size_t bind_offset = 0);
	void copy(void* src, size_t copy_size);
	void upload(void* src, size_t src_size, size_t dst_offset = 0);

	MemoryInfo queryMemoryInfo(vk::MemoryPropertyFlags properties) const;
};
