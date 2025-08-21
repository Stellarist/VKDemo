#include "Application.hpp"

constexpr int SCR_WIDTH = 2560;
constexpr int SCR_HEIGHT = 1440;

Application::Application() :
    window("VKDemo", SCR_WIDTH, SCR_HEIGHT),
    instance(),
    surface(window, instance),
    device(instance, surface),
    swap_chain(window, device, surface),
    render_pass(device, swap_chain),
    graphics_pipeline(device, render_pass),
    command_pool(device),
    command_buffer(device, swap_chain, render_pass, graphics_pipeline, command_pool),
    sync_objects(device)
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

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			graphics_pipeline.render(&swap_chain, &command_buffer, &sync_objects);

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

		device.getLogicalDevice().waitIdle();
	}
}
