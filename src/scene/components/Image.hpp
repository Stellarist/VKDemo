#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "scene/Component.hpp"

class Image : public Component {
private:
	std::vector<uint8_t> data;

	uint32_t format{0};
	uint32_t width{0};
	uint32_t height{0};

public:
	Image(const std::string& name);

	Image(const Image& other) = delete;
	Image& operator=(const Image& other) = delete;

	Image(Image&& other) noexcept = default;
	Image& operator=(Image&& other) noexcept = default;

	~Image() override = default;

	std::type_index getType() override;

	auto getData() const -> const std::vector<uint8_t>&;
	void setData(const std::vector<uint8_t>& new_data);

	auto getFormat() const -> uint32_t;
	void setFormat(uint32_t new_format);

	auto getWidth() const -> uint32_t;
	void setWidth(uint32_t new_width);

	auto getHeight() const -> uint32_t;
	void setHeight(uint32_t new_height);
};
