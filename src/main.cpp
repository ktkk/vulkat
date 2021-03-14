#include <ctime>

#include <iostream>
#include <cstdlib>

#include "core/core.hpp"

using namespace vulkat;

int main(int argc, char **argv){
	srand(time(nullptr));

	// Create a new core object on the heap
	Core* pCore{ new Core{ Window{ "WindowName", 1280.f, 720.f } } };

	try {
		pCore->TestVulkan(); // Print the number of extensions supported
		pCore->Run(); // Run the game loop
	} catch (const std::exception& e) {
		std::cerr << "Exception caught: '" << e.what() << "'/n";
		return EXIT_FAILURE;
	}

	delete pCore;

	return EXIT_SUCCESS;
}
