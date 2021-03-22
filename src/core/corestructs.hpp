#ifndef CORESTRUCTS_HPP
#define CORESTRUCTS_HPP

namespace vulkat{
	struct Window{
		explicit Window(const std::string& title = "WindowTitle", float width = 720.f, float height = 480.f, bool isVsyncOn = true);

		std::string title;
		float width, height;
		bool isVsyncOn;
	};
}
#endif // CORESTRUCTS_HPP
