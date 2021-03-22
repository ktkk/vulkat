#ifndef CORESTRUCTS_HPP
#define CORESTRUCTS_HPP

#include <optional> // c++17 data structure to distinguish between the case of a value existing or not

namespace vulkat{
	struct Window {
		explicit Window(const std::string& title = "WindowTitle", float width = 720.f, float height = 480.f, bool isVsyncOn = true);

		std::string title;
		float width, height;
		bool isVsyncOn;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily; // Allows for checking if a value is present

		bool IsComplete();
	};
}
#endif // CORESTRUCTS_HPP
