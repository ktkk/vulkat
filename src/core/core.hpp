#ifndef CORE_HPP
#define CORE_HPP

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// Window
#include "window.hpp"

// std
#include <iostream>

namespace vulkat{
	class Core final {
	public:
		explicit Core(const Window& window);
		
		// Disallow copy
		Core(const Core& other) = delete;
		Core(Core&& other) = delete;
		Core& operator=(const Core& other) = delete;
		Core& operator=(Core&& other) = delete;

		// destructor
		~Core();

		void TestVulkan();
		void Run();

	private:
		// DATA MEMBERS
		const Window m_Window; // Window properties
		GLFWwindow* m_pWindow;

		// MEMBER FUNCTIONS
		void Initialize();
		void Cleanup();
	};
}
#endif // CORE_HPP
