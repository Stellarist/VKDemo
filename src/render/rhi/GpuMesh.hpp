#pragma once

#include <vulkan/vulkan.hpp>

#include "GpuUniforms.hpp"
#include "render/graphics/Buffer.hpp"
#include "scene/components/SubMesh.hpp"

class GpuMesh {
private:
	std::unique_ptr<Buffer> vertex_buffer;
	std::unique_ptr<Buffer> index_buffer;

	std::vector<GpuUniforms> uniform_data;

	uint32_t vertex_count{};
	uint32_t index_count{};

	const SubMesh* submesh{};

	Context* context{};

public:
	GpuMesh(Context& context, const SubMesh& submesh);

	GpuMesh(const GpuMesh&) = delete;
	GpuMesh& operator=(const GpuMesh&) = delete;

	GpuMesh(GpuMesh&&) noexcept = default;
	GpuMesh& operator=(GpuMesh&&) noexcept = default;

	~GpuMesh() = default;

	void draw();

	vk::Buffer getVertexBuffer() const;
	vk::Buffer getIndexBuffer() const;

	uint32_t getVertexCount() const;
	uint32_t getIndexCount() const;
};
