#pragma once
#include "window.hpp"

class Core final {
public:
	explicit Core(const Window& window);
	
	// Disallow copy
	Core(const Core& other) = delete;
	Core(Core&& other) = delete;
	Core& operator=(const Core& other) = delete;
	Core& operator=(Core&& other) = delete;

	~Core();

	void Run();

private:
	const Window m_Window; // Window properties

}
