#pragma once
#include <string>

struct Window{
	explicit Window(const std::string& title = "WindowTitle", float width = 720.f, float height 480.f, bool isVsyncOn = true)
		: title{ title }
		, width{ width }
		, height{ height }
		, isVsyncOn{ isVsyncOn }
	{};

	std::string title;
	float width, height;
	bool isVsyncOn;
}
