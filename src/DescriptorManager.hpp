#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

class DescriptorManager {
private:
	std::vector<vk::DescriptorPool> descriptor_pool;
	std::vector<vk::DescriptorSet>  descriptor_sets;

	Context* context;

	void createPool();

public:
	DescriptorManager(Context& context);
	~DescriptorManager();

	std::pair<vk::DescriptorSet, uint32_t>            allocateSet();
	std::pair<std::span<vk::DescriptorSet>, uint32_t> allocateSets(uint32_t count);

	void freeSet(vk::DescriptorSet set);
	void freeSets(std::span<vk::DescriptorSet> sets);

	void begin(vk::CommandBuffer command_buffer);
	void end(vk::CommandBuffer command_buffer);

	void resetPool(vk::CommandPoolResetFlags flags = {});

	vk::CommandPool   getPool() const;
	vk::CommandBuffer getBuffer(uint32_t index) const;
};

class DescriptorManager {
public:
	// 描述符池配置
	struct PoolConfig {
		uint32_t max_sets = 1000;                          // 最大描述符集数量
		uint32_t uniform_buffers = 1000;                   // Uniform Buffer 数量
		uint32_t storage_buffers = 1000;                   // Storage Buffer 数量
		uint32_t combined_image_samplers = 1000;           // 纹理采样器数量
		uint32_t storage_images = 100;                     // Storage Image 数量
		uint32_t samplers = 100;                           // 单独采样器数量
		bool     allow_free_descriptor_sets = true;        // 是否允许单独释放描述符集
	};

private:
	VkDevice                                               device_;
	std::vector<VkDescriptorPool>                          pools_;
	std::unordered_map<std::string, VkDescriptorSetLayout> layouts_;

	// 当前使用的池索引
	size_t current_pool_index_;

	// 池配置
	PoolConfig pool_config_;

	// 内部方法
	VkDescriptorPool create_pool(const PoolConfig& config);
	void             cleanup_pools();

public:
	DescriptorManager();
	~DescriptorManager();

	// 禁用拷贝
	DescriptorManager(const DescriptorManager&) = delete;
	DescriptorManager& operator=(const DescriptorManager&) = delete;

	// 初始化和清理
	bool initialize(VkDevice device, const PoolConfig& config = {});
	void shutdown();

	// 创建描述符集布局
	VkDescriptorSetLayout create_layout(const std::string&                               name,
	                                    const std::vector<VkDescriptorSetLayoutBinding>& bindings);

	// 获取已创建的布局
	VkDescriptorSetLayout get_layout(const std::string& name) const;

	// 分配描述符集
	VkDescriptorSet allocate_set(VkDescriptorSetLayout layout);
	VkDescriptorSet allocate_set(const std::string& layout_name);

	// 批量分配描述符集
	std::vector<VkDescriptorSet> allocate_sets(VkDescriptorSetLayout layout, uint32_t count);

	// 更新描述符集（绑定资源）
	void update_set(VkDescriptorSet set, uint32_t binding, VkBuffer buffer,
	                VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
	void update_set(VkDescriptorSet set, uint32_t binding, VkImageView image_view,
	                VkSampler sampler, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// 重置池（释放所有描述符集）
	void reset_pool(size_t pool_index = 0);
	void reset_all_pools();

	// 创建新的池（如果当前池满了）
	bool create_additional_pool();

	// 辅助方法：创建常用的描述符集布局
	VkDescriptorSetLayout create_uniform_buffer_layout(const std::string& name);
	VkDescriptorSetLayout create_texture_layout(const std::string& name);
	VkDescriptorSetLayout create_storage_buffer_layout(const std::string& name);

	// 获取信息
	bool              is_initialized() const { return device_ != VK_NULL_HANDLE; }
	size_t            get_pool_count() const { return pools_.size(); }
	const PoolConfig& get_pool_config() const { return pool_config_; }
};

// 辅助函数：创建常用的绑定
namespace DescriptorHelpers
{
// 创建 Uniform Buffer 绑定
VkDescriptorSetLayoutBinding uniform_buffer_binding(uint32_t           binding,
                                                    VkShaderStageFlags stage_flags);

// 创建纹理绑定
VkDescriptorSetLayoutBinding texture_binding(uint32_t           binding,
                                             VkShaderStageFlags stage_flags);

// 创建 Storage Buffer 绑定
VkDescriptorSetLayoutBinding storage_buffer_binding(uint32_t           binding,
                                                    VkShaderStageFlags stage_flags);
}        // namespace DescriptorHelpers
