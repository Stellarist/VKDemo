#include "Object.hpp"

uint64_t Object::id_counter = 0;

Object::Object() :
    uid(id_counter++)
{}

uint64_t Object::getUid() const
{
	return uid;
}