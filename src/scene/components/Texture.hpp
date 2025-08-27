#pragma once

#include <string>
#include <typeindex>
#include <cassert>

#include "Image.hpp"
#include "scene/Component.hpp"

class Texture : public Component {
private:
	Image* image{nullptr};

public:
	Texture(const std::string& name);

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other) noexcept = default;
	Texture& operator=(Texture&& other) noexcept = default;

	virtual ~Texture() = default;

	virtual std::type_index getType() override;

	auto getImage() -> Image*;
	void setImage(Image& image);
};
