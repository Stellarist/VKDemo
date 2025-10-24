#pragma once

#include <memory>
#include <string>
#include <vector>
#include <typeindex>
#include <algorithm>

#include "Node.hpp"

class Scene : public Entity {
private:
	std::string name;

	Node* root{};

	std::vector<std::unique_ptr<Node>> nodes;

	std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> components;

	std::vector<std::unique_ptr<Behaviour>> behaviours;
	std::vector<Behaviour*>                 tickable_behaviours;

public:
	Scene() = default;
	Scene(std::string name);
	Scene(int id, const std::string& name);

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) noexcept = default;
	Scene& operator=(Scene&&) noexcept = default;

	~Scene() override = default;

	std::type_index getType() override;

	auto getName() const -> const std::string&;
	void setName(const std::string& name);

	void setNodes(std::vector<std::unique_ptr<Node>>&& nodes);
	void addNode(std::unique_ptr<Node>&& node);

	Node& getRoot();
	void  setRoot(Node& node);
	void  addChild(Node& child);

	template <typename T>
	auto getComponents() const -> std::vector<T*>;
	auto getComponents(const std::type_index& type) const -> const std::vector<std::unique_ptr<Component>>&;

	template <typename T>
	void setComponents(std::vector<std::unique_ptr<T>>&& components);
	void setComponents(const std::type_index& type, std::vector<std::unique_ptr<Component>>&& components);

	void addComponent(std::unique_ptr<Component>&& component);
	void addComponent(std::unique_ptr<Component>&& component, Node& node);

	template <typename T>
	void clearComponents();
	void removeComponent(Component& component);

	template <typename T>
	bool hasComponent() const;
	bool hasComponent(const std::type_index& type) const;

	template <typename T>
	auto getBehaviour() const -> T*;
	auto getBehaviours() const -> const std::vector<std::unique_ptr<Behaviour>>&;

	void addBehaviour(std::unique_ptr<Behaviour>&& behaviour);
	void addBehaviour(std::unique_ptr<Behaviour>&& behaviour, Node& node);

	void removeBehaviour(Behaviour& behaviour);
	void refreshBehaviours();

	Node* findNode(const std::string& name);

	void start();
	void update(float dt);
};

template <typename T>
auto Scene::getComponents() const -> std::vector<T*>
{
	std::vector<T*> result;
	if (hasComponent(typeid(T))) {
		auto& scene_components = getComponents(typeid(T));
		result.resize(scene_components.size());
		std::transform(scene_components.begin(), scene_components.end(), result.begin(), [](auto& component) -> T* {
			return dynamic_cast<T*>(component.get());
		});
	}
	return result;
}

template <typename T>
void Scene::setComponents(std::vector<std::unique_ptr<T>>&& components)
{
	std::vector<std::unique_ptr<Component>> result(components.size());
	std::transform(components.begin(), components.end(), result.begin(), [](auto& component) -> std::unique_ptr<Component> {
		return std::unique_ptr<Component>(std::move(component));
	});
	setComponents(typeid(T), std::move(result));
}

template <typename T>
void Scene::clearComponents()
{
	setComponents<T>(typeid(T), {});
}

template <typename T>
bool Scene::hasComponent() const
{
	return hasComponent(typeid(T));
}

template <typename T>
T* Scene::getBehaviour() const
{
	for (auto& behaviour : behaviours)
		if (auto* result = dynamic_cast<T*>(behaviour.get()))
			return result;

	return nullptr;
}
