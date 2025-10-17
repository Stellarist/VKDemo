#include "render/Renderer.hpp"
#include "gui/Window.hpp"
#include "resource/SceneLoader.hpp"

int main(int argc, char** argv)
{
	auto scene = SceneLoader::loadScene(ASSETS_DIR "/teapot.gltf");
	SceneLoader::printSceneNodes(*scene);
	SceneLoader::printSceneComponents(*scene);

	auto submeshes = scene->getComponents<SubMesh>();
	for (size_t i = 0; i < submeshes.size(); ++i) {
		const auto& submesh = *submeshes[i];
		SceneLoader::printSubmeshInfo(submesh);
		SceneLoader::exportSubmeshToOBJ(submesh, std::format("submesh_{}.obj", i));
		SceneLoader::printSubmeshDetailed(submesh);
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
