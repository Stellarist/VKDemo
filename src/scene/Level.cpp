#include "Level.hpp"

Level::Level(std::unique_ptr<Scene>&& new_scene)
{
	setActiveScene(std::move(new_scene));
}

void Level::tick(float dt)
{
	if (!active_scene)
		return;

	active_scene->update(dt);
}

Scene* Level::getActiveScene() const
{
	return active_scene.get();
}

void Level::setActiveScene(std::unique_ptr<Scene>&& new_scene)
{
	active_scene = std::move(new_scene);
	if (!active_scene)
		return;

	active_scene->start();
	auto cameras = active_scene->getComponents<Camera>();
	if (!cameras.empty())
		active_camera = cameras.front();
}

Camera* Level::getActiveCamera() const
{
	return active_camera;
}

void Level::setActiveCamera(Camera* camera)
{
	active_camera = camera;
}