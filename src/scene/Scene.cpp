#include "Scene.hpp"

#include <queue>

#include "components/SubMesh.hpp"

Scene::Scene(std::string name) :
    name(std::move(name))
{}

Scene::Scene(int id, const std::string& name) :
    root(new Node(id, name)),
    name(name)
{}

std::type_index Scene::getType()
{
	return typeid(Scene);
}

const std::string& Scene::getName() const
{
	return name;
}

void Scene::setName(const std::string& name)
{
	this->name = name;
}

void Scene::setNodes(std::vector<std::unique_ptr<Node>>&& nodes)
{
	assert(!nodes.empty() && "Nodes cannot be empty.");
	this->nodes = std::move(nodes);
}

void Scene::addNode(std::unique_ptr<Node>&& node)
{
	nodes.emplace_back(std::move(node));
}

void Scene::addChild(Node& child)
{
	root->addChild(child);
}

auto Scene::getModel(unsigned int index) -> std::unique_ptr<Component>
{
	auto meshed = std::move(components.at(typeid(SubMesh)));

	assert(index < meshed.size() && "Index out of bounds.");
	return std::move(meshed[index]);
}

auto Scene::getComponents(const std::type_index& type) const -> const std::vector<std::unique_ptr<Component>>&
{
	return components.at(type);
}

void Scene::setComponents(const std::type_index& type, std::vector<std::unique_ptr<Component>>&& components)
{
	this->components[type] = std::move(components);
}

void Scene::addComponent(std::unique_ptr<Component>&& component, Node& node)
{
	node.setComponent(*component);
	if (component)
		components[component->getType()].emplace_back(std::move(component));
}

void Scene::addComponent(std::unique_ptr<Component>&& component)
{
	if (component)
		components[component->getType()].emplace_back(std::move(component));
}

void Scene::removeComponent(Component& component)
{
	auto it = components.find(component.getType());
	if (it == components.end())
		return;

	auto& component_list = it->second;
	component_list.erase(std::remove_if(component_list.begin(), component_list.end(),
	                                    [&component](const std::unique_ptr<Component>& c) {
		                                    return c.get() == &component;
	                                    }),
	                     component_list.end());

	if (component_list.empty())
		components.erase(it);
}

bool Scene::hasComponent(const std::type_index& type) const
{
	return components.count(type) > 0;
}

Node* Scene::findNode(const std::string& name)
{
	for (auto* node : root->getChildren()) {
		std::queue<Node*> traverse_node;
		traverse_node.push(node);

		while (!traverse_node.empty()) {
			auto* node = traverse_node.front();
			traverse_node.pop();
			if (node->getName() == name)
				return node;

			for (auto* child_node : node->getChildren())
				traverse_node.push(child_node);
		}
	}
	return nullptr;
}

void Scene::setRoot(Node& node)
{
	this->root = &node;
}

Node& Scene::getRoot()
{
	return *root;
}
