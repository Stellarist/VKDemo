#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "Buffer.hpp"
#include "Image.hpp"

template <>
struct std::hash<vk::DescriptorPool> {
	size_t operator()(vk::DescriptorPool pool) const noexcept
	{
		return std::hash<uint64_t>()(reinterpret_cast<uint64_t>(static_cast<vk::DescriptorPool::CType>(pool)));
	}
};

class DescriptorManager {
private:
	std::vector<vk::DescriptorSetLayout>                                   descriptor_layouts;
	std::unordered_map<vk::DescriptorPool, std::vector<vk::DescriptorSet>> descriptor_map;

	Context* context{};

public:
	DescriptorManager(Context& context);

	DescriptorManager(const DescriptorManager&) = delete;
	DescriptorManager& operator=(const DescriptorManager&) = delete;

	DescriptorManager(DescriptorManager&&) noexcept = default;
	DescriptorManager& operator=(DescriptorManager&&) noexcept = default;

	~DescriptorManager();

	vk::DescriptorPool      createPool(vk::DescriptorType type, uint32_t max_sets);
	vk::DescriptorSetLayout createLayout(std::span<const vk::DescriptorSetLayoutBinding> bindings);

	vk::DescriptorSet              allocateSet(vk::DescriptorPool pool, const vk::DescriptorSetLayout& layout);
	vk::DescriptorSet              allocateSet(vk::DescriptorPool pool, std::span<const vk::DescriptorSetLayoutBinding> bindings);
	std::vector<vk::DescriptorSet> allocateSets(vk::DescriptorPool pool, std::span<const vk::DescriptorSetLayout> layouts);

	void freeSet(vk::DescriptorPool pool, vk::DescriptorSet set);
	void freeSets(vk::DescriptorPool pool, std::span<const vk::DescriptorSet> sets);

	void updateSet(vk::DescriptorSet set, uint32_t binding, vk::DescriptorType type, const Buffer* buffer = {});
	void updateSet(vk::DescriptorSet set, uint32_t binding, vk::DescriptorType type, const Image* texture = {});

	bool hasPool(vk::DescriptorPool pool) const;
	void resetPool(vk::DescriptorPool pool);

	const std::vector<vk::DescriptorSet>& getSets(vk::DescriptorPool pool) const;
};
