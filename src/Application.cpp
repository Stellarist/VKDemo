#include "Application.hpp"

constexpr int SCR_WIDTH = 2560;
constexpr int SCR_HEIGHT = 1440;

Application::Application() :
    window("VKDemo", SCR_WIDTH, SCR_HEIGHT),
    renderer(window)
{
}

void Application::run()
{
	bool should_close = false;
	while (!should_close) {
		static uint64_t last_tick{}, current_tick{};
		current_tick = SDL_GetTicks();
		float delta_time = static_cast<float>(current_tick - last_tick) / 1000.0f;
		last_tick = current_tick;

		renderer.begin();
		renderer.render();
		renderer.end();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EventType::SDL_EVENT_QUIT:
				should_close = true;
				break;
			case SDL_EventType::SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE) {
					should_close = true;
					break;
				}
			}
		}

		renderer.wait();
	}
}
