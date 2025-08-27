#pragma once

#include <string>
#include <typeindex>

#include "Entity.hpp"

class Component : public Entity {
private:
	std::string name;

public:
	Component() = default;
	Component(std::string name);

	Component(const Component&) = default;
	Component& operator=(Component&) = default;

	Component(Component&&) noexcept = default;
	Component& operator=(Component&&) noexcept = default;

	~Component() override = default;

	std::type_index getType() override = 0;

	auto getName() const -> const std::string&;
	void setName(const std::string& name);
};
