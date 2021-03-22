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
		Core(const Core& other) = delete; // Copy constructor
		Core(Core&& other) = delete; // Move constructor
		Core& operator=(const Core& other) = delete; // Copy assignment
		Core& operator=(Core&& other) = delete; // Move assignment

		// destructor
		~Core();

		void Run();

	private:
		// DATA MEMBERS
		const Window m_Window; // Window properties
		bool m_Debug;

		GLFWwindow* m_pWindow; // Window to render to
		VkInstance m_pInstance; // Vulkan Instance (is pointer)
		VkDebugUtilsMessengerEXT m_pDebugMessenger; // Debug Messenger (is pointer)
		VkPhysicalDevice m_PhysicalDevice;

		// MEMBER FUNCTIONS
		void Initialize();
		void CreateInstance();
		void Cleanup();

		// Vulkan Extension & Validation layer checks
		void PrintVulkanExtensions() const;
		std::vector<const char*> GetRequiredExtensions();
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		// Debug messenger
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void SetupDebugMessenger();

		// Physical devices and Queue families
		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		uint32_t FindQueueFamilies(VkPhysicalDevice device);
	};
}
#endif // CORE_HPP
