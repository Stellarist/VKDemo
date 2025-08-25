#include "Window.hpp"
#include "Renderer.hpp"

int main(int argc, char** argv)
{
	Window   window("VKDemo", 2560, 1440);
	Renderer renderer(window);
	// Widget   widget();

	while (!window.shouldClose()) {
		renderer.render();
		window.pollEvents();
		renderer.wait();
	}

	return 0;
}
