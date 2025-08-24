#pragma once

#include "core/Renderer.hpp"
#include "Window.hpp"
#include "Widget.hpp"

class Application {
private:
	Window   window;
	Renderer renderer;
	Widget   widget;

public:
	Application();

	void run();
};
