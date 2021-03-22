#include "../pch.hpp"
#include "corestructs.hpp"

namespace vulkat{
	Window::Window(const std::string& title, float width, float height, bool isVsyncOn)
		: title{ title }
		, width{ width }
		, height{ height }
		, isVsyncOn{ isVsyncOn }
	{}
}
