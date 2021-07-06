#ifndef CORE_HPP
#define CORE_HPP

// Corestructs
#include "corestructs.hpp"

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
		const Window m_WindowProperties; // Window properties
		bool m_Debug;

		GLFWwindow* m_pWindow; // Window to render to
		VkInstance m_pInstance; // Vulkan Instance (is pointer)
		VkDebugUtilsMessengerEXT m_pDebugMessenger; // Debug Messenger (is pointer)
		VkSurfaceKHR m_Surface; // Window Surface
		VkPhysicalDevice m_PhysicalDevice; // Physical device (the GPU)
		VkDevice m_Device; // Logical device
		VkQueue m_GraphicsQueue; // Handle to interact with graphics queue
		VkQueue m_PresentQueue; // Handle to interact with the presentation queue
		VkSwapchainKHR m_SwapChain; // Swapchain
		std::vector<VkImage> m_SwapChainImages; // Handle for images in swapchain
		std::vector<VkImageView> m_SwapChainImageViews;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews; // Handle for image views in swapchain

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
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		// Logical devices
		void CreateLogicalDevice();

		// Window Surface
		void CreateSurface();

		// Swap Chain
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormats(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapChain();

		// Image Views
		void CreateImageViews();

		// Graphics pipeline
		void CreateGraphicsPipeline();
	};
}
#endif // CORE_HPP
