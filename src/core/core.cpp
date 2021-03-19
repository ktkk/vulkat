#include "../pch.hpp"
#include "core.hpp"

namespace vulkat{
	// CreateDebugUtilsMessengerEXT Proxy function (GLOBAL FUNCTION, maybe throw this in wrapper class "Validation")
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
					      const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		// Look for the address of the function
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	
		// if the function exists, return it, otherwise throw an error
		if (func) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	// DestroyDebugUtilsMessengerEXT Proxy function (GLOBAL FUNCTION, maybe throw in wrapper class "Validation")
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator) {
		// Look for the address of the function
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		// if the function exists, return it
		if (func) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	Core::Core(const Window& window, bool debug)
		: m_Window{ window }
		, m_Debug{ debug }
		, m_pWindow{ nullptr }
	{
		Initialize();
	}

	Core::~Core() {
		Cleanup();
	}

	void Core::Run() {
		while(!glfwWindowShouldClose(m_pWindow)) {
			glfwPollEvents();
		}
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

		// Create a vulkan instance
		CreateInstance();
		SetupDebugMessenger();
	}

	void Core::CreateInstance() {
		if (m_Debug && !Validation::CheckValidationLayerSupport()) {
			// Check if validation layers are supported
			throw std::runtime_error("Validation layer requested, but none available!");
		}

		// Optional: Info about the app
		// APPINFO
		VkApplicationInfo appInfo{}; // pNext gets initialized to nullptr
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_Window.title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = ENGINE;
		appInfo.engineVersion = VK_MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		// END APPINFO

		// Not Optional: Vulkan extensions and validation layers
		// CREATEINFO
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		
		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (m_Debug) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(Validation::m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = Validation::m_ValidationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}
		// END CREATEINFO

		// Create the instance
		if (vkCreateInstance(&createInfo, nullptr, &m_pInstance) != VK_SUCCESS) {
			// Throw an rte if Instance creation failed
			throw std::runtime_error("Failed to create VK instance!\n");
		}

		if(m_Debug){ // If debug flag is specified, print the extensions
			PrintVulkanExtensions();
		}
	}

	void Core::Cleanup() {
		if (m_Debug) {
			DestroyDebugUtilsMessengerEXT(m_pInstance, m_DebugMessenger, nullptr);
		}

		vkDestroyInstance(m_pInstance, nullptr);

		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	// Extension support
	void Core::PrintVulkanExtensions() const {
		// Print nr of extensions and their names to the console
		uint32_t extensionCount{ 0 };
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::cout << extensionCount << " extensions supported!\n";

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "Available extensions:\n";
		for (const auto& extension : extensions) {
			std::cout << "\t" << extension.extensionName << "\n";
		}
	}

	std::vector<const char*> Core::GetRequiredExtensions() {
		// Returns required extensions if debug enabled
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if(m_Debug) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			// VK_EXT_DEBUG_UTILS_EXTENSION_NAME = "VK_EXT_debug_utils"
		}

		return extensions;
	}

	// Debugger
	VKAPI_ATTR VkBool32 VKAPI_CALL Core::DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;

		// messageSeverity:
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: diagnostic message
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: informational message
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: warning message
		// 	Likely a bug
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: error message
		// 	May result in crash

		// can optionally be checked like so:
		// if (messageSeverity >= VK_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		//	// show the message
		// }

		// messageType:
		// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: unrelated to specification or performance
		// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: violates specification -> possible mistake
		// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: non-optimal use of vulkan

		// messageData:
		// pMessage: debug message
		// pObjects: Vulkan object handles related to the message
		// objectCount: Nr of objects in pObjects

		// pUserData: allows for passing of own data
	}

	void Core::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	void Core::SetupDebugMessenger() {
		if(!m_Debug) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		// Initialize the debug messenger
		if (CreateDebugUtilsMessengerEXT(m_pInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}
}
