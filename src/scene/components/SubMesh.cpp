#include "SubMesh.hpp"

SubMesh::SubMesh(const std::string& name) :
    Component{name}
{}

std::type_index SubMesh::getType()
{
	return typeid(SubMesh);
}

uint32_t SubMesh::getVerticesCount() const
{
	return vertices_count;
}

uint32_t SubMesh::getIndicesCount() const
{
	return indices_count;
}

auto SubMesh::getVertices() const -> const std::vector<float>&
{
	return vertex_data;
}

void SubMesh::setVertices(const std::vector<float>& vertex_data, uint32_t count)
{
	this->vertex_data = vertex_data;
	vertices_count = count;
}

void SubMesh::setVertices(std::vector<float>&& vertex_data, uint32_t count)
{
	this->vertex_data = std::move(vertex_data);
	vertices_count = count;
}

auto SubMesh::getIndices() const -> const std::vector<uint32_t>&
{
	return index_data;
}

void SubMesh::setIndices(const std::vector<uint32_t>& index_data)
{
	this->index_data = index_data;
	indices_count = this->index_data.size();
}

void SubMesh::setIndices(std::vector<uint32_t>&& index_data)
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
