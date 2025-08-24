#include "DescriptorManager.hpp"

#include "DescriptorManager.hpp"
#include <stdexcept>
#include <iostream>

// DescriptorManager::DescriptorManager() :
//     device_(VK_NULL_HANDLE), current_pool_index_(0)
// {
// }

// DescriptorManager::~DescriptorManager()
// {
// 	if (device_ != VK_NULL_HANDLE) {
// 		shutdown();
// 	}
// }

// bool DescriptorManager::initialize(VkDevice device, const PoolConfig& config)
// {
// 	if (device_ != VK_NULL_HANDLE) {
// 		std::cerr << "DescriptorManager already initialized!" << std::endl;
// 		return false;
// 	}

// 	device_ = device;
// 	pool_config_ = config;

// 	try {
// 		// 创建第一个描述符池
// 		VkDescriptorPool pool = create_pool(config);
// 		pools_.push_back(pool);
// 		current_pool_index_ = 0;

// 		std::cout << "DescriptorManager initialized with pool capacity: "
// 		          << config.max_sets << " sets" << std::endl;
// 		return true;

// 	} catch (const std::exception& e) {
// 		std::cerr << "Failed to initialize DescriptorManager: " << e.what() << std::endl;
// 		return false;
// 	}
// }

// void DescriptorManager::shutdown()
// {
// 	if (device_ == VK_NULL_HANDLE) {
// 		return;
// 	}

// 	// 等待设备空闲
// 	vkDeviceWaitIdle(device_);

// 	// 清理描述符集布局
// 	for (auto& [name, layout] : layouts_) {
// 		vkDestroyDescriptorSetLayout(device_, layout, nullptr);
// 	}
// 	layouts_.clear();

// 	// 清理描述符池
// 	cleanup_pools();

// 	device_ = VK_NULL_HANDLE;
// }

// VkDescriptorPool DescriptorManager::create_pool(const PoolConfig& config)
// {
// 	std::vector<VkDescriptorPoolSize> pool_sizes;

// 	// 添加各种类型的描述符
// 	if (config.uniform_buffers > 0) {
// 		pool_sizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, config.uniform_buffers});
// 	}
// 	if (config.storage_buffers > 0) {
// 		pool_sizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, config.storage_buffers});
// 	}
// 	if (config.combined_image_samplers > 0) {
// 		pool_sizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, config.combined_image_samplers});
// 	}
// 	if (config.storage_images > 0) {
// 		pool_sizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, config.storage_images});
// 	}
// 	if (config.samplers > 0) {
// 		pool_sizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLER, config.samplers});
// 	}

// 	VkDescriptorPoolCreateInfo pool_info = {};
// 	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
// 	pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
// 	pool_info.pPoolSizes = pool_sizes.data();
// 	pool_info.maxSets = config.max_sets;

// 	if (config.allow_free_descriptor_sets) {
// 		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
// 	}

// 	VkDescriptorPool pool;
// 	VkResult         result = vkCreateDescriptorPool(device_, &pool_info, nullptr, &pool);
// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("Failed to create descriptor pool");
// 	}

// 	return pool;
// }

// void DescriptorManager::cleanup_pools()
// {
// 	for (VkDescriptorPool pool : pools_) {
// 		vkDestroyDescriptorPool(device_, pool, nullptr);
// 	}
// 	pools_.clear();
// }

// VkDescriptorSetLayout DescriptorManager::create_layout(const std::string&                               name,
//                                                        const std::vector<VkDescriptorSetLayoutBinding>& bindings)
// {
// 	// 检查是否已存在
// 	auto it = layouts_.find(name);
// 	if (it != layouts_.end()) {
// 		std::cerr << "Layout '" << name << "' already exists!" << std::endl;
// 		return it->second;
// 	}

// 	VkDescriptorSetLayoutCreateInfo layout_info = {};
// 	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
// 	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
// 	layout_info.pBindings = bindings.data();

// 	VkDescriptorSetLayout layout;
// 	VkResult              result = vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &layout);
// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("Failed to create descriptor set layout: " + name);
// 	}

// 	layouts_[name] = layout;
// 	return layout;
// }

// VkDescriptorSetLayout DescriptorManager::get_layout(const std::string& name) const
// {
// 	auto it = layouts_.find(name);
// 	if (it == layouts_.end()) {
// 		return VK_NULL_HANDLE;
// 	}
// 	return it->second;
// }

// VkDescriptorSet DescriptorManager::allocate_set(VkDescriptorSetLayout layout)
// {
// 	if (current_pool_index_ >= pools_.size()) {
// 		throw std::runtime_error("No available descriptor pools");
// 	}

// 	VkDescriptorSetAllocateInfo alloc_info = {};
// 	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
// 	alloc_info.descriptorPool = pools_[current_pool_index_];
// 	alloc_info.descriptorSetCount = 1;
// 	alloc_info.pSetLayouts = &layout;

// 	VkDescriptorSet set;
// 	VkResult        result = vkAllocateDescriptorSets(device_, &alloc_info, &set);

// 	if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
// 		// 当前池已满，尝试创建新池
// 		if (create_additional_pool()) {
// 			alloc_info.descriptorPool = pools_[current_pool_index_];
// 			result = vkAllocateDescriptorSets(device_, &alloc_info, &set);
// 		}
// 	}

// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("Failed to allocate descriptor set");
// 	}

// 	return set;
// }

// VkDescriptorSet DescriptorManager::allocate_set(const std::string& layout_name)
// {
// 	VkDescriptorSetLayout layout = get_layout(layout_name);
// 	if (layout == VK_NULL_HANDLE) {
// 		throw std::runtime_error("Layout not found: " + layout_name);
// 	}
// 	return allocate_set(layout);
// }

// std::vector<VkDescriptorSet> DescriptorManager::allocate_sets(VkDescriptorSetLayout layout, uint32_t count)
// {
// 	std::vector<VkDescriptorSetLayout> layouts(count, layout);
// 	std::vector<VkDescriptorSet>       sets(count);

// 	VkDescriptorSetAllocateInfo alloc_info = {};
// 	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
// 	alloc_info.descriptorPool = pools_[current_pool_index_];
// 	alloc_info.descriptorSetCount = count;
// 	alloc_info.pSetLayouts = layouts.data();

// 	VkResult result = vkAllocateDescriptorSets(device_, &alloc_info, sets.data());
// 	if (result != VK_SUCCESS) {
// 		throw std::runtime_error("Failed to allocate descriptor sets");
// 	}

// 	return sets;
// }

// void DescriptorManager::update_set(VkDescriptorSet set, uint32_t binding, VkBuffer buffer,
//                                    VkDeviceSize offset, VkDeviceSize range)
// {
// 	VkDescriptorBufferInfo buffer_info = {};
// 	buffer_info.buffer = buffer;
// 	buffer_info.offset = offset;
// 	buffer_info.range = range;

// 	VkWriteDescriptorSet write = {};
// 	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 	write.dstSet = set;
// 	write.dstBinding = binding;
// 	write.dstArrayElement = 0;
// 	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
// 	write.descriptorCount = 1;
// 	write.pBufferInfo = &buffer_info;

// 	vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
// }

// void DescriptorManager::update_set(VkDescriptorSet set, uint32_t binding, VkImageView image_view,
//                                    VkSampler sampler, VkImageLayout layout)
// {
// 	VkDescriptorImageInfo image_info = {};
// 	image_info.imageLayout = layout;
// 	image_info.imageView = image_view;
// 	image_info.sampler = sampler;

// 	VkWriteDescriptorSet write = {};
// 	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
// 	write.dstSet = set;
// 	write.dstBinding = binding;
// 	write.dstArrayElement = 0;
// 	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
// 	write.descriptorCount = 1;
// 	write.pImageInfo = &image_info;

// 	vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
// }

// void DescriptorManager::reset_pool(size_t pool_index)
// {
// 	if (pool_index < pools_.size()) {
// 		vkResetDescriptorPool(device_, pools_[pool_index], 0);
// 	}
// }

// void DescriptorManager::reset_all_pools()
// {
// 	for (VkDescriptorPool pool : pools_) {
// 		vkResetDescriptorPool(device_, pool, 0);
// 	}
// }

// bool DescriptorManager::create_additional_pool()
// {
// 	try {
// 		VkDescriptorPool new_pool = create_pool(pool_config_);
// 		pools_.push_back(new_pool);
// 		current_pool_index_ = pools_.size() - 1;

// 		std::cout << "Created additional descriptor pool. Total pools: "
// 		          << pools_.size() << std::endl;
// 		return true;
// 	} catch (const std::exception& e) {
// 		std::cerr << "Failed to create additional descriptor pool: " << e.what() << std::endl;
// 		return false;
// 	}
// }

// // 辅助方法：创建常用布局
// VkDescriptorSetLayout DescriptorManager::create_uniform_buffer_layout(const std::string& name)
// {
// 	std::vector<VkDescriptorSetLayoutBinding> bindings = {
// 	    DescriptorHelpers::uniform_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)};
// 	return create_layout(name, bindings);
// }

// VkDescriptorSetLayout DescriptorManager::create_texture_layout(const std::string& name)
// {
// 	std::vector<VkDescriptorSetLayoutBinding> bindings = {
// 	    DescriptorHelpers::texture_binding(0, VK_SHADER_STAGE_FRAGMENT_BIT)};
// 	return create_layout(name, bindings);
// }

// VkDescriptorSetLayout DescriptorManager::create_storage_buffer_layout(const std::string& name)
// {
// 	std::vector<VkDescriptorSetLayoutBinding> bindings = {
// 	    DescriptorHelpers::storage_buffer_binding(0, VK_SHADER_STAGE_COMPUTE_BIT)};
// 	return create_layout(name, bindings);
// }

// // 辅助函数实现
// namespace DescriptorHelpers
// {
// VkDescriptorSetLayoutBinding uniform_buffer_binding(uint32_t binding, VkShaderStageFlags stage_flags)
// {
// 	VkDescriptorSetLayoutBinding ubo_binding = {};
// 	ubo_binding.binding = binding;
// 	ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
// 	ubo_binding.descriptorCount = 1;
// 	ubo_binding.stageFlags = stage_flags;
// 	ubo_binding.pImmutableSamplers = nullptr;
// 	return ubo_binding;
// }

// VkDescriptorSetLayoutBinding texture_binding(uint32_t binding, VkShaderStageFlags stage_flags)
// {
// 	VkDescriptorSetLayoutBinding sampler_binding = {};
// 	sampler_binding.binding = binding;
// 	sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
// 	sampler_binding.descriptorCount = 1;
// 	sampler_binding.stageFlags = stage_flags;
// 	sampler_binding.pImmutableSamplers = nullptr;
// 	return sampler_binding;
// }

// VkDescriptorSetLayoutBinding storage_buffer_binding(uint32_t binding, VkShaderStageFlags stage_flags)
// {
// 	VkDescriptorSetLayoutBinding storage_binding = {};
// 	storage_binding.binding = binding;
// 	storage_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
// 	storage_binding.descriptorCount = 1;
// 	storage_binding.stageFlags = stage_flags;
// 	storage_binding.pImmutableSamplers = nullptr;
// 	return storage_binding;
// }
// }        // namespace DescriptorHelpers