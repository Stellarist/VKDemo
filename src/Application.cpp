#include "Application.hpp"

constexpr int SCR_WIDTH = 2560;
constexpr int SCR_HEIGHT = 1440;

Application::Application() :
    window("VKDemo", SCR_WIDTH, SCR_HEIGHT),
    context(window),
    swap_chain(window, context),
    render_pass(context, swap_chain),
    graphics_pipeline(context, render_pass),
    command_manager(context, swap_chain, render_pass, graphics_pipeline),
    sync_objects(context)
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

		context.getLogicalDevice().waitIdle();
	}
}
