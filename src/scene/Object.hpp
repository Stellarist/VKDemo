#pragma once

#include <typeindex>

class Object {
private:
	uint64_t uid;

	static uint64_t id_counter;

public:
	Object();

	Object(const Object&) = default;
	Object& operator=(Object&) = default;

	Object(Object&&) noexcept = default;
	Object& operator=(Object&&) noexcept = default;

	virtual ~Object() = default;

	uint64_t getUid() const;

	virtual std::type_index getType() = 0;
};
