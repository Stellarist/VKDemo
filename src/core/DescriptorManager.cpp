#include "DescriptorManager.hpp"

DescriptorManager::DescriptorManager(Context& context) :
    context(&context)
{
}

DescriptorManager::~DescriptorManager()
{
	for (auto& pool : descriptor_pool)
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
	descriptor_pool.push_back(pool);

	return pool;
}

std::vector<vk::DescriptorSet> DescriptorManager::allocateSets(std::span<const vk::DescriptorSetLayoutBinding> bindings)
{
	descriptor_sets.reserve(descriptor_sets.size() + bindings.size());

	for (const auto& binding : bindings) {
		// if (!context->getLogicalDevice().createDescriptorSetLayout(layout_info))
		// 	throw std::runtime_error("Failed to create descriptor set layout");
	}

	// return {sets_size, bindings.size()};
}
