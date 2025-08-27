#include "SubMesh.hpp"

SubMesh::SubMesh(const std::string& name) :
    Component{name}
{}

std::type_index SubMesh::getType()
{
	return typeid(SubMesh);
}

unsigned int SubMesh::getVerticesCount() const
{
	return vertices_count;
}

unsigned int SubMesh::getIndicesCount() const
{
	return indices_count;
}

auto SubMesh::getVertices() const -> const std::vector<float>&
{
	return vertex_data;
}

void SubMesh::setVertices(const std::vector<float>& vertex_data, unsigned int count)
{
	this->vertex_data = vertex_data;
	vertices_count = count;
}

void SubMesh::setVertices(std::vector<float>&& vertex_data, unsigned int count)
{
	this->vertex_data = std::move(vertex_data);
	vertices_count = count;
}

auto SubMesh::getIndices() const -> const std::vector<unsigned int>&
{
	return index_data;
}

void SubMesh::setIndices(const std::vector<unsigned int>& index_data)
{
	this->index_data = index_data;
	indices_count = this->index_data.size();
}

void SubMesh::setIndices(std::vector<unsigned int>&& index_data)
{
	this->index_data = std::move(index_data);
	indices_count = this->index_data.size();
}

auto SubMesh::getAttributes() const -> const std::unordered_map<std::string, VertexAttribute>&
{
	return vertex_attributes;
}

bool SubMesh::getAttribute(const std::string& attribute_name, VertexAttribute& attribute) const
{
	auto attrib_it = vertex_attributes.find(attribute_name);
	if (attrib_it == vertex_attributes.end())
		return false;

	attribute = attrib_it->second;
	return true;
}

void SubMesh::setAttribute(const std::string& attribute_name, const VertexAttribute& attribute)
{
	vertex_attributes[attribute_name] = attribute;
}

void SubMesh::setAttribute(const std::string& attribute_name, VertexAttribute&& attribute)
{
	vertex_attributes[attribute_name] = std::move(attribute);
}

const Material* SubMesh::getMaterial() const
{
	return material;
}

void SubMesh::setMaterial(const Material& new_material)
{
	material = &new_material;
}

const std::string& SubMesh::getShaderName() const
{
	return shader_name;
}

void SubMesh::setShaderName(const std::string& shader_name)
{
	this->shader_name = shader_name;
}

bool SubMesh::isVisible() const
{
	return visible;
}

void SubMesh::setVisible(bool visible)
{
	this->visible = visible;
}
