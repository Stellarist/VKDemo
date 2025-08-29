#include "render/Renderer.hpp"
#include "gui/Window.hpp"

int main(int argc, char** argv)
{
	// auto scene = SceneLoader::loadScene(ASSETS_DIR "/teapot.gltf");

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
