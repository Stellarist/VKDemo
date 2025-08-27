#include "Node.hpp"

Node::Node(size_t id, std::string name) :
    id(id),
    name(std::move(name)),
    transform(*this)
{
	setComponent(transform);
}

std::type_index Node::getType()
{
	return typeid(Node);
}

size_t Node::getId() const
{
	return id;
}

const std::string& Node::getName() const
{
	return name;
}

void Node::setName(const std::string& name)
{
	this->name = name;
}

Node* Node::getParent() const
{
	return parent;
}

void Node::setParent(Node& parent)
{
	this->parent = &parent;
	transform.invalidateWorldMatrix();
	if (std::find(parent.getChildren().begin(), parent.getChildren().end(), this) == parent.getChildren().end())
		parent.children.push_back(this);
}

Transform& Node::getTransform()
{
	return transform;
}

Component& Node::getComponent(std::type_index type) const
{
	return *components.at(type);
}

void Node::setComponent(Component& component)
{
	components[component.getType()] = &component;
}

bool Node::hasComponent(std::type_index type) const
{
	return components.count(type) > 0;
}

const std::vector<Node*>& Node::getChildren() const
{
	return children;
}

void Node::addChild(Node& child)
{
	children.push_back(&child);
	child.setParent(*this);
}
