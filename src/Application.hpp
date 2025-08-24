#pragma once

#include "Window.hpp"
#include "Renderer.hpp"

class Application {
private:
	Window   window;
	Renderer renderer;

public:
	Application();

	void run();
};
