#include <ctime>
#include "core/core.hpp"

using namespace vulkat;

int main(int argc, char **argv){
	srand(time(nullptr));

	// Create a new core object on the heap
	Core* pCore{ new Core{ Window{ "WindowName", 1280.f, 720.f } } };
	pCore->Run(); // Run the game loop
	delete pCore;

	return 0;
}

// TODO:
/*
- Memory leak detection
-

*/
