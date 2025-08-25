#pragma once

#include <string_view>

#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

class Window {
private:
	SDL_Window* window{};

	std::string title{};
	uint32_t    width{};
	uint32_t    height{};
	bool        should_close{};

public:
	Window(std::string_view title, int width, int height);
	~Window();

	void pollEvents();

	std::string getTitle() const;
	uint32_t    getWidth() const;
	uint32_t    getHeight() const;
	bool        shouldClose() const;

	void setTitle(std::string_view title);
	void setWidth(uint32_t width);
	void setHeight(uint32_t height);

	SDL_Window* get() const;
};
