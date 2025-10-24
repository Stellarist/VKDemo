#include "Level.hpp"

Scene* Level::getActiveScene()
{
	return active_scene.get();
}

void Level::setActiveScene(std::unique_ptr<Scene>&& scene)
{
	active_scene = std::move(scene);
}

Camera* Level::getActiveCamera()
{
	return active_camera;
}

void Level::setActiveCamera(Camera* camera)
{
	active_camera = camera;
}