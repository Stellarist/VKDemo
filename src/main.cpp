#include "Application.hpp"

#include <print>

int main(int argc, char** argv)
{
	try {
		Application().run();
	} catch (const std::exception& e) {
		std::println("Error: {}", e.what());
	}

	return 0;
}
