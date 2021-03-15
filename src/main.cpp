#include <ctime>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>

#include "core/core.hpp"

using namespace vulkat;

bool debug{ false };
std::string helpMsg{
	"Options:\n"
	"\t-d :\tToggle Vulkan debug messages\n"
	"\t-h :\tDisplay this help\n"
};

int main(int argc, char* argv[]){
	srand(time(nullptr));

	int option;
	while((option = getopt(argc, argv, "dh")) != -1) {
		switch(option){
		case 'd':
			debug = true;
			break;
		case 'h':
		default:
			std::cout << helpMsg << '\n';
			break;
		}
	}

	// Create a new core object on the heap
	Core* pCore{ new Core{ Window{ "WindowName", 1280.f, 720.f }, debug } };

	try {
		pCore->Run(); // Run the game loop
	} catch (const std::exception& e) {
		std::cerr << "Exception caught: '" << e.what() << "'/n";
		return EXIT_FAILURE;
	}

	delete pCore;

	return EXIT_SUCCESS;
}
