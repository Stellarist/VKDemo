#include "render/Renderer.hpp"
#include "gui/Window.hpp"
#include "resource/SceneLoader.hpp"
#include "utils/helper.hpp"

int main(int argc, char** argv)
{
	auto scene = SceneLoader::loadScene(ASSETS_DIR "/teapot.gltf");
	printSceneNodes(*scene);
	printSceneComponents(*scene);

	for (const auto& submesh : scene->getComponents<SubMesh>()) {
		printSubmeshInfo(*submesh);
		exportSubmeshToOBJ(*submesh, std::format("submesh_{}.obj", submesh->getName()));
		printSubmeshDetailed(*submesh);
	}

	Window   window("VKDemo", 2560, 1440);
	Renderer renderer(window);

	// Widget widget(window, *renderer.context, *renderer.render_pass);

	while (!window.shouldClose()) {
		window.pollEvents();
		renderer.begin();
		renderer.draw();
		renderer.end();
		renderer.wait();
	}

	return 0;
}
