#include "Renderer.hpp"

#include <print>

int main(int argc, char** argv)
{
	try {
		Renderer().run();
	} catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}
