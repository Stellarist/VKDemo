#include "AABB.hpp"

AABB::AABB() :
    min(0.0f),
    max(0.0f)
{
	reset();
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max) :
    min(min),
    max(max)
{}

std::type_index AABB::getType()
{
	return typeid(AABB);
}

void AABB::update(const glm::vec3& point)
{
	min = glm::min(min, point);
	max = glm::max(max, point);
}

void AABB::update(const std::vector<glm::vec3>& vertex_data, const std::vector<unsigned int>& index_data)
{
	if (index_data.size() > 0)
		for (unsigned int index_id : index_data)
			update(vertex_data[index_id]);
	else
		for (auto vertex_id : vertex_data)
			update(vertex_id);
}

void AABB::transform(const glm::mat4& transform)
{
	min = max = glm::vec4(min, 1.0f) * transform;
	update(glm::vec4(min.x, min.y, max.z, 1.0f) * transform);
	update(glm::vec4(min.x, max.y, min.z, 1.0f) * transform);
	update(glm::vec4(min.x, max.y, max.z, 1.0f) * transform);
	update(glm::vec4(max.x, min.y, min.z, 1.0f) * transform);
	update(glm::vec4(max.x, min.y, max.z, 1.0f) * transform);
	update(glm::vec4(max.x, max.y, min.z, 1.0f) * transform);
	update(glm::vec4(max, 1.0f) * transform);
}

void AABB::reset()
{
	min = std::numeric_limits<glm::vec3>::max();
	max = std::numeric_limits<glm::vec3>::min();
}

glm::vec3 AABB::getScale() const
{
	return (max - min);
}

glm::vec3 AABB::getCenter() const
{
	return (min + max) * 0.5f;
}

glm::vec3 AABB::getMin() const
{
	return min;
}

glm::vec3 AABB::getMax() const
{
	return max;
}
