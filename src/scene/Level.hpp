#pragma once

#include <memory>

#include "base/Scene.hpp"
#include "components/Camera.hpp"

class Level {
private:
	std::unique_ptr<Scene> active_scene;

	Camera* active_camera{};

public:
	Level() = default;

	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;

	Level(Level&&) noexcept = default;
	Level& operator=(Level&&) noexcept = default;

	~Level() = default;

	auto getActiveScene() -> Scene*;
	void setActiveScene(std::unique_ptr<Scene>&& scene);

	auto getActiveCamera() -> Camera*;
	void setActiveCamera(Camera* camera);
};
