#include "GpuMesh.hpp"

#include <numeric>

#include "render/graphics/Context.hpp"
#include "render/graphics/Buffer.hpp"
#include "scene/components/SubMesh.hpp"
#include "GpuVertex.hpp"

GpuMesh::GpuMesh(Context& context, const SubMesh& submesh) :
    context(&context), submesh(&submesh)
{
	const auto& vertices = submesh.getVertices();
	const auto& indices = submesh.getIndices();
	const auto& attributes = submesh.getAttributes();

	vertex_count = submesh.getVerticesCount();
	index_count = submesh.getIndicesCount();

	if (!vertices.empty() && vertex_count > 0) {
		auto source_stride = std::accumulate(attributes.begin(), attributes.end(), 0u,
		                                     [](uint32_t sum, const auto& pair) {
			                                     return sum + pair.second.size;
		                                     });

		const auto* pos_attribute = submesh.getAttribute("POSITION");
		const auto* normal_attribute = submesh.getAttribute("NORMAL");
		const auto* uv_attribute = submesh.getAttribute("TEXCOORD_0");
		const auto* color_attribute = submesh.getAttribute("COLOR_0");

		std::vector<GpuVertex> gpu_vertices(vertex_count);
		const uint8_t*         src_data = reinterpret_cast<const uint8_t*>(vertices.data());

		for (uint32_t i = 0; i < vertex_count; i++) {
			const uint8_t* vertex_data = src_data + i * source_stride;

			if (pos_attribute)
				std::memcpy(&gpu_vertices[i].pos, vertex_data + pos_attribute->offset, sizeof(glm::vec3));
			else
				gpu_vertices[i].pos = glm::vec3(0.0f);

			if (normal_attribute)
				std::memcpy(&gpu_vertices[i].normal, vertex_data + normal_attribute->offset, sizeof(glm::vec3));
			else
				gpu_vertices[i].normal = glm::vec3(0.0f, 0.0f, 1.0f);

			if (uv_attribute)
				std::memcpy(&gpu_vertices[i].uv, vertex_data + uv_attribute->offset, sizeof(glm::vec2));
			else
				gpu_vertices[i].uv = glm::vec2(0.0f);

			if (color_attribute)
				std::memcpy(&gpu_vertices[i].color, vertex_data + color_attribute->offset, sizeof(glm::vec4));
			else
				gpu_vertices[i].color = glm::vec4(1.0f);
		}

		vertex_buffer = Buffer::createFrom(
		    *this->context,
		    vk::BufferUsageFlagBits::eVertexBuffer,
		    gpu_vertices.data(),
		    gpu_vertices.size() * sizeof(GpuVertex));
	}

	if (!indices.empty()) {
		index_buffer = Buffer::createFrom(
		    *this->context,
		    vk::BufferUsageFlagBits::eIndexBuffer,
		    indices.data(),
		    indices.size() * sizeof(uint32_t));
	}
}

void GpuMesh::draw()
{
}

vk::Buffer GpuMesh::getVertexBuffer() const
{
	return vertex_buffer->get();
}

vk::Buffer GpuMesh::getIndexBuffer() const
{
	return index_buffer->get();
}

uint32_t GpuMesh::getVertexCount() const
{
	return vertex_count;
}

uint32_t GpuMesh::getIndexCount() const
{
	return index_count;
}
