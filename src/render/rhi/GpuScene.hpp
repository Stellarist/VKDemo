#pragma once

#include <memory>
#include <vector>

#include "GpuMesh.hpp"
#include "render/graphics/Context.hpp"
#include "scene/base/Scene.hpp"

class GpuScene {
private:
	Context*     context{nullptr};
	const Scene* scene{nullptr};

	std::vector<std::unique_ptr<GpuMesh>> gpu_meshes;

public:
	GpuScene() = default;
	GpuScene(Context& context, const Scene& scene);

	GpuScene(const GpuScene&) = delete;
	GpuScene& operator=(const GpuScene&) = delete;

	GpuScene(GpuScene&&) noexcept = default;
	GpuScene& operator=(GpuScene&&) noexcept = default;

	~GpuScene() = default;

	void update();
	void draw(vk::CommandBuffer command_buffer);

	auto getGpuMeshes() const -> const std::vector<std::unique_ptr<GpuMesh>>&;
};
