#include "Mesh.hpp"

Mesh::Mesh(const std::string& name) :
    Component{name}
{}

std::type_index Mesh::getType()
{
	return typeid(Mesh);
}

Node* Mesh::getNode() const
{
	return node;
}

void Mesh::setNode(Node& node)
{
	this->node = &node;
}

const AABB& Mesh::getBounds() const
{
	return bounds;
}

void Mesh::updateBounds(const std::vector<glm::vec3>& vertex_data, const std::vector<unsigned int>& index_data)
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
