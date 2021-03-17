#ifndef CORE_HPP
#define CORE_HPP

// Window
#include "window.hpp"

// Validation Layers
#include "validation.hpp"

// vulkat
#define ENGINE "vulkat"
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 1

namespace vulkat{
	class Core final {
	public:
		explicit Core(const Window& window, bool debug);
		
		// Disallow copy
		Core(const Core& other) = delete;
		Core(Core&& other) = delete;
		Core& operator=(const Core& other) = delete;
		Core& operator=(Core&& other) = delete;

		// destructor
		~Core();

		void Run();

	private:
		// DATA MEMBERS
		const Window m_Window; // Window properties
		bool m_Debug;

		GLFWwindow* m_pWindow; // Window to render to
		VkInstance m_pInstance; // Vulkan Instance (is pointer)

		// MEMBER FUNCTIONS
		void Initialize();
		void CreateInstance();
		void TestVulkanExtensions() const;
		void Cleanup();
	};
}
#endif // CORE_HPP
