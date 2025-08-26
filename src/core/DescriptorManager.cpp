#include "DescriptorManager.hpp"

#include "Buffer.hpp"

DescriptorManager::DescriptorManager(Context& context) :
    context(&context)
{
}

DescriptorManager::~DescriptorManager()
{
	for (auto& [pool, sets] : descriptor_map)
		context->getLogicalDevice().destroyDescriptorPool(pool);
}

vk::DescriptorSetLayout DescriptorManager::createLayout(std::span<const vk::DescriptorSetLayoutBinding> bindings)
{
	vk::DescriptorSetLayoutCreateInfo create_info{};
	create_info.setBindings(bindings);

	auto layout = context->getLogicalDevice().createDescriptorSetLayout(create_info);

	return layout;
}

vk::DescriptorPool DescriptorManager::createPool(vk::DescriptorType type, uint32_t max_sets)
{
	vk::DescriptorPoolSize pool_size{};
	pool_size.setType(type)
	    .setDescriptorCount(max_sets);

	vk::DescriptorPoolCreateInfo create_info{};
	create_info.setPoolSizes(pool_size)
	    .setMaxSets(max_sets);

	auto pool = context->getLogicalDevice().createDescriptorPool(create_info);

	descriptor_map[pool].reserve(max_sets);

	return pool;
}

vk::DescriptorSet DescriptorManager::allocateSet(vk::DescriptorPool pool, const vk::DescriptorSetLayout& layout)
{
	return allocateSets(pool, {&layout, 1}).front();
}

vk::DescriptorSet DescriptorManager::allocateSet(vk::DescriptorPool pool, std::span<const vk::DescriptorSetLayoutBinding> bindings)
{
	auto layout = createLayout(bindings);
	return allocateSets(pool, {&layout, 1}).front();
}

std::vector<vk::DescriptorSet> DescriptorManager::allocateSets(vk::DescriptorPool pool, std::span<const vk::DescriptorSetLayout> layouts)
{
	if (descriptor_map.find(pool) == descriptor_map.end())
		throw std::runtime_error("Descriptor pool not found");

	if (descriptor_map[pool].size() + layouts.size() > descriptor_map[pool].capacity())
		throw std::runtime_error("Descriptor pool capacity exceeded");

	vk::DescriptorSetAllocateInfo alloc_info{};
	alloc_info.setDescriptorPool(pool)
	    .setDescriptorSetCount(layouts.size())
	    .setSetLayouts(layouts);

	auto sets = context->getLogicalDevice().allocateDescriptorSets(alloc_info);
	descriptor_map[pool].insert(descriptor_map[pool].end(), sets.begin(), sets.end());

	return sets;
}

void DescriptorManager::freeSet(vk::DescriptorPool pool, vk::DescriptorSet set)
{
	freeSets(pool, {&set, 1});
}

void DescriptorManager::freeSets(vk::DescriptorPool pool, std::span<const vk::DescriptorSet> sets)
{
	if (descriptor_map.find(pool) == descriptor_map.end())
		throw std::runtime_error("Descriptor pool not found");

	context->getLogicalDevice().freeDescriptorSets(pool, sets);

	descriptor_map[pool].erase(
	    std::remove_if(
	        descriptor_map[pool].begin(),
	        descriptor_map[pool].end(),
	        [&sets](const vk::DescriptorSet& set) {
		        return std::find(sets.begin(), sets.end(), set) != sets.end();
	        }),
	    descriptor_map[pool].end());
}

void DescriptorManager::updateSet(vk::DescriptorSet set, uint32_t binding, vk::DescriptorType type, const Buffer* buffer)
{
	vk::WriteDescriptorSet write{};
	write.setDstSet(set)
	    .setDstBinding(binding)
	    .setDstArrayElement(0)
	    .setDescriptorType(type)
	    .setDescriptorCount(1);

	if (buffer) {
		vk::DescriptorBufferInfo buffer_info{};
		buffer_info.setBuffer(buffer->get())
		    .setOffset(0)
		    .setRange(buffer->getSize());

		write.setBufferInfo(buffer_info);
	}

	context->getLogicalDevice().updateDescriptorSets(write, {});
}

bool DescriptorManager::hasPool(vk::DescriptorPool pool) const
{
	return descriptor_map.find(pool) != descriptor_map.end();
}

void DescriptorManager::resetPool(vk::DescriptorPool pool)
{
	if (descriptor_map.find(pool) != descriptor_map.end())
		context->getLogicalDevice().resetDescriptorPool(pool);

	descriptor_map[pool].clear();
}

const std::vector<vk::DescriptorSet>& DescriptorManager::getSets(vk::DescriptorPool pool) const
{
	auto it = descriptor_map.find(pool);
	if (it != descriptor_map.end())
		return it->second;

	throw std::runtime_error("Descriptor pool not found");
}
