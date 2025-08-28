#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "scene/Component.hpp"

class AABB : public Component {
private:
	glm::vec3 min;
	glm::vec3 max;

public:
	AABB();
	AABB(const glm::vec3& min, const glm::vec3& max);

	AABB(const AABB&) = delete;
	AABB& operator=(const AABB&) = delete;

	AABB(AABB&&) noexcept = default;
	AABB& operator=(AABB&&) noexcept = default;

	~AABB() override = default;

	std::type_index getType() override;

	void update(const glm::vec3& point);
	void update(const std::vector<glm::vec3>& vertex_data, const std::vector<uint32_t>& index_data);
	void transform(const glm::mat4& transform);
	void reset();

	glm::vec3 getScale() const;
	glm::vec3 getCenter() const;
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
};
