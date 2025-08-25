#pragma once

#include <vulkan/vulkan.hpp>

#include "core/Context.hpp"

struct DescriptorInfo {
	uint32_t pool_index;
	uint32_t set_index;
};

class DescriptorManager {
private:
	std::vector<vk::DescriptorPool> descriptor_pool;
	std::vector<vk::DescriptorSet>  descriptor_sets;

	Context* context;

	void createPool();

public:
	DescriptorManager(Context& context);
	~DescriptorManager();

	vk::DescriptorSetLayout createLayout(std::span<const vk::DescriptorSetLayoutBinding> bindings);

	vk::DescriptorPool createPool(vk::DescriptorType type, uint32_t max_sets);

	vk::DescriptorSet              allocateSet(const vk::DescriptorSetLayoutBinding& binding);
	std::vector<vk::DescriptorSet> allocateSets(std::span<const vk::DescriptorSetLayoutBinding> bindings);

	void freeSet(vk::DescriptorSet set);
	void freeSets(std::span<vk::DescriptorSet> sets);
};
