#include "Window.hpp"
#include "Renderer.hpp"

int main(int argc, char** argv)
{
	Window   window("VKDemo", 2560, 1440);
	Renderer renderer(window);
	// Widget   widget(window, *renderer.context, *renderer.render_pass);

	while (!window.shouldClose()) {
		window.pollEvents();
		renderer.render();
		renderer.wait();
	}

	return 0;
}
