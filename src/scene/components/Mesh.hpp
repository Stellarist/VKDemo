#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "AABB.hpp"
#include "SubMesh.hpp"
#include "scene/base/Component.hpp"
#include "scene/base/Node.hpp"

class Mesh : public Component {
private:
	AABB                  bounds;
	std::vector<Node*>    nodes;
	std::vector<SubMesh*> submeshes;

public:
	Mesh(const std::string& name);

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(Mesh&&) noexcept = default;

	~Mesh() override = default;

	std::type_index getType() override;

	auto getNodes() const -> const std::vector<Node*>&;
	void addNode(Node& node);

	auto getBounds() const -> const AABB&;
	void updateBounds(const std::vector<glm::vec3>& vertex_data, const std::vector<uint32_t>& index_data = {});

	auto getSubmeshes() const -> const std::vector<SubMesh*>&;
	void addSubmesh(SubMesh& submesh);
};
