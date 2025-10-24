#include "GpuScene.hpp"

#include "render/rhi/GpuMesh.hpp"
#include "scene/components/SubMesh.hpp"

GpuScene::GpuScene(Context& context, const Scene& scene) :
    context(&context), scene(&scene)
{
	auto submeshes = scene.getComponents<SubMesh>();

	gpu_meshes.reserve(submeshes.size());
	for (const auto* submesh : submeshes)
		if (submesh && submesh->isVisible())
			gpu_meshes.push_back(std::make_unique<GpuMesh>(*this->context, *submesh));
}

void GpuScene::draw(vk::CommandBuffer command_buffer)
{
	for (size_t i = 0; i < gpu_meshes.size(); i++)
		if (auto& gpu_mesh = gpu_meshes[i]; gpu_mesh) {
			command_buffer.bindVertexBuffers(0, gpu_mesh->getVertexBuffer(), {0});
			command_buffer.bindIndexBuffer(gpu_mesh->getIndexBuffer(), 0, vk::IndexType::eUint32);
			command_buffer.drawIndexed(gpu_mesh->getIndexCount(), 1, 0, 0, 0);
		}
}
