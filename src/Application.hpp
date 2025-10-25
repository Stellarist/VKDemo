#pragma once

#include <memory>

#include "gui/Window.hpp"
#include "render/Renderer.hpp"
#include "scene/Level.hpp"

class Application {
private:
	std::unique_ptr<Window>   window;
	std::unique_ptr<Level>    level;
	std::unique_ptr<Renderer> renderer;

	float last_time{0.0f};
	float delta_time{0.0f};

public:
	Application();
	~Application() = default;

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	Application(Application&&) noexcept = default;
	Application& operator=(Application&&) noexcept = default;

	void run();
	void elapseTime();

	void tickGui(float dt);
	void tickLogic(float dt);
	void tickRender(float dt);

	void loadLevel(std::unique_ptr<Level>&& level);
	void loadRenderer(std::unique_ptr<Renderer>&& renderer);
};
