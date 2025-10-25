#include "Application.hpp"

#include <chrono>

#include "resource/SceneLoader.hpp"

Application::Application()
{
	auto scene = SceneLoader::loadScene(ASSETS_DIR "/teapot.gltf");
	window = std::make_unique<Window>("VKEngine", 2560, 1440);
	level = std::make_unique<Level>();
	level->setActiveScene(std::move(scene));
	renderer = std::make_unique<Renderer>(*window);
	renderer->setActiveLevel(*level);
}

void Application::run()
{
	while (!window->shouldClose()) {
		elapseTime();

		tickGui(delta_time);
		tickLogic(delta_time);
		tickRender(delta_time);
	}
}

void Application::elapseTime()
{
	float current_time = std::chrono::duration<float>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	delta_time = current_time - last_time;
	last_time = current_time;
}

void Application::tickGui(float dt)
{
	window->pollEvents();
}

void Application::tickLogic(float dt)
{
	if (level)
		level->tick(dt);
}

void Application::tickRender(float dt)
{
	if (renderer)
		renderer->tick(dt);
}

void Application::loadLevel(std::unique_ptr<Level>&& new_level)
{
	level = std::move(new_level);
	if (level)
		renderer->setActiveLevel(*level);
}

void Application::loadRenderer(std::unique_ptr<Renderer>&& new_renderer)
{
	renderer = std::move(new_renderer);
	if (level)
		renderer->setActiveLevel(*level);
}
