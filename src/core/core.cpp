#include "core.hpp"

namespace vulkat{
	Core::Core(const Window& window)
		: m_Window{ window }
		, m_pWindow{ nullptr }
	{
		Initialize();
	}

	Core::~Core() {
		Cleanup();
	}

	void Core::TestVulkan() {
		uint32_t extensionCount{ 0 };
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::cout << extensionCount << " extensions supported!\n";
	}

	void Run() {
		// while(!glfwWindowShouldClose(m_pWindow)) {
		// 	glfwPollEvents();
		// }
	}

	void Core::Initialize() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // disable opengl
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // disable resizing

		m_pWindow = glfwCreateWindow(
				m_Window.width,
				m_Window.height,
				m_Window.title.c_str(),
				nullptr, // not fullscreen
				nullptr ); // don't share resources with other windows (opengl only)
	}

	void Core::Cleanup() {
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}
}
