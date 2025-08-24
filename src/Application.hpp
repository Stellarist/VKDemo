#pragma once

#include "Window.hpp"
#include "core/Renderer.hpp"

class Application {
private:
	Window   window;
	Renderer renderer;

public:
	Application();

	void run();
};
