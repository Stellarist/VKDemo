#include "Mesh.hpp"

Mesh::Mesh(const std::string& name) :
    Component{name}
{}

std::type_index Mesh::getType()
{
	return typeid(Mesh);
}

const std::vector<Node*>& Mesh::getNodes() const
{
	return nodes;
}

void Mesh::addNode(Node& node)
{
	nodes.push_back(&node);
}

const AABB& Mesh::getBounds() const
{
	return bounds;
}

void Mesh::updateBounds(const std::vector<glm::vec3>& vertex_data, const std::vector<uint32_t>& index_data)
{
	bounds.update(vertex_data, index_data);
}

const std::vector<SubMesh*>& Mesh::getSubmeshes() const
{
	return submeshes;
}

void Mesh::addSubmesh(SubMesh& submesh)
{
	submeshes.push_back(&submesh);
}
