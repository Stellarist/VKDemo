#pragma once

#include <memory>
#include <vector>

#include "scene/base/Scene.hpp"
#include "rhi/GpuMesh.hpp"
#include "graphics/Context.hpp"

class RenderScene {
private:
	Context*     context{nullptr};
	const Scene* scene{nullptr};

	std::vector<std::unique_ptr<GpuMesh>> gpu_meshes;

public:
	RenderScene() = default;
	RenderScene(Context& context, const Scene& scene);

	RenderScene(const RenderScene&) = delete;
	RenderScene& operator=(const RenderScene&) = delete;

	RenderScene(RenderScene&&) noexcept = default;
	RenderScene& operator=(RenderScene&&) noexcept = default;

	~RenderScene() = default;

	void update();
	void draw(vk::CommandBuffer command_buffer);

	auto getGpuMeshes() const -> const std::vector<std::unique_ptr<GpuMesh>>&;
};
