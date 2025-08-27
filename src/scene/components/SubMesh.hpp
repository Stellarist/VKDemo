#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "scene/Component.hpp"
#include "Material.hpp"

struct VertexAttribute {
	int          format = 0;
	unsigned int count = 0;
	unsigned int stride = 0;
	unsigned int offset = 0;
};

class SubMesh : public Component {
private:
	const Material* material{nullptr};
	std::string     shader_name;

	unsigned int vertices_count{0};
	unsigned int indices_count{0};

	std::vector<float>        vertex_data;
	std::vector<unsigned int> index_data;

	std::unordered_map<std::string, VertexAttribute> vertex_attributes;

	bool visible{true};

public:
	SubMesh(const std::string& name = {});

	SubMesh(const SubMesh&) = delete;
	SubMesh& operator=(const SubMesh&) = delete;

	SubMesh(SubMesh&&) noexcept = default;
	SubMesh& operator=(SubMesh&&) noexcept = default;

	~SubMesh() override = default;

	std::type_index getType() override;

	unsigned int getVerticesCount() const;
	unsigned int getIndicesCount() const;

	auto getVertices() const -> const std::vector<float>&;
	void setVertices(const std::vector<float>& vertex_data, unsigned int count);
	void setVertices(std::vector<float>&& vertex_data, unsigned int count);

	auto getIndices() const -> const std::vector<unsigned int>&;
	void setIndices(const std::vector<unsigned int>& index_data);
	void setIndices(std::vector<unsigned int>&& index_data);

	auto getAttributes() const -> const std::unordered_map<std::string, VertexAttribute>&;
	bool getAttribute(const std::string& name, VertexAttribute& attribute) const;
	void setAttribute(const std::string& name, const VertexAttribute& attribute);
	void setAttribute(const std::string& name, VertexAttribute&& attribute);

	auto getMaterial() const -> const Material*;
	void setMaterial(const Material& material);

	auto getShaderName() const -> const std::string&;
	void setShaderName(const std::string& shader_name);

	bool isVisible() const;
	void setVisible(bool visible);
};
