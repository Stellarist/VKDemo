#include "Buffer.hpp"

Buffer::Buffer(Context& context, uint32_t size, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags properties) :
    context(&context), size(size)
{
	create(usage, size);
	allocate(properties);
	bind();
}

Buffer::~Buffer()
{
	context->getLogicalDevice().freeMemory(memory);
	context->getLogicalDevice().destroyBuffer(buffer);
}

void Buffer::create(vk::BufferUsageFlagBits usage, size_t size)
{
	vk::BufferCreateInfo create_info{};
	create_info.setSize(size)
	    .setUsage(usage)
	    .setSharingMode(vk::SharingMode::eExclusive);

	buffer = context->getLogicalDevice().createBuffer(create_info);
}

void Buffer::allocate(vk::MemoryPropertyFlags properties)
{
	MemoryInfo info = queryMemoryInfo(properties);

	vk::MemoryAllocateInfo allocate_info{};
	allocate_info.setAllocationSize(info.size)
	    .setMemoryTypeIndex(info.index);

	memory = context->getLogicalDevice().allocateMemory(allocate_info);
}

void Buffer::map(size_t map_size, size_t map_offset)
{
	data = context->getLogicalDevice().mapMemory(memory, map_offset, map_size);
}

void Buffer::unmap()
{
	if (data) {
		context->getLogicalDevice().unmapMemory(memory);
		data = nullptr;
	}
}

void Buffer::bind(size_t bind_offset)
{
	context->getLogicalDevice().bindBufferMemory(buffer, memory, bind_offset);
}

void Buffer::copy(void* src, size_t copy_size)
{
	std::memcpy(data, src, copy_size);
}

void Buffer::upload(void* src, size_t src_size, size_t dst_offset)
{
	map(src_size, dst_offset);
	copy(src, src_size);
	unmap();
}

vk::Buffer Buffer::get() const
{
	return buffer;
}

MemoryInfo Buffer::queryMemoryInfo(vk::MemoryPropertyFlags prop_flags) const
{
	MemoryInfo info{};

	auto requirements = context->getLogicalDevice().getBufferMemoryRequirements(buffer);
	info.size = requirements.size;

	auto properties = context->getPhysicalDevice().getMemoryProperties();
	for (auto i = 0; i < properties.memoryTypeCount; i++)
		if (requirements.memoryTypeBits & (1 << i) && (properties.memoryTypes[i].propertyFlags & prop_flags)) {
			info.index = i;
			break;
		}

	return info;
}
