#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "AABB.hpp"
#include "SubMesh.hpp"
#include "scene/Component.hpp"
#include "scene/Node.hpp"

class Mesh : public Component {
private:
	Node*                 node{nullptr};
	AABB                  bounds;
	std::vector<SubMesh*> submeshes;

public:
	Mesh(const std::string& name);

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(Mesh&&) noexcept = default;

	~Mesh() override = default;

	std::type_index getType() override;

	auto getNode() const -> Node*;
	void setNode(Node& node);

	auto getBounds() const -> const AABB&;
	void updateBounds(const std::vector<glm::vec3>& vertex_data, const std::vector<unsigned int>& index_data = {});

	auto getSubmeshes() const -> const std::vector<SubMesh*>&;
	void addSubmesh(SubMesh& submesh);
};
