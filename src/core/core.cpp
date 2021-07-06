#include "../pch.hpp"
#include "core.hpp"

namespace debug {
	// CreateDebugUtilsMessengerEXT Proxy function (GLOBAL FUNCTION, maybe throw this in wrapper class "Validation")
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		// Look for the address of the function
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	
		// if the function exists, return it, otherwise throw an error
		if(func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	// DestroyDebugUtilsMessengerEXT Proxy function (GLOBAL FUNCTION, maybe throw in wrapper class "Validation")
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		// Look for the address of the function
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		// if the function exists, return it
		if(func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}
}

namespace vulkat{
	Core::Core(const Window& window, bool debug)
		: m_WindowProperties{ window }
		, m_Debug{ debug }
		, m_pWindow{ nullptr }
		, m_pInstance{ nullptr }
		, m_pDebugMessenger{ nullptr }
		, m_PhysicalDevice{ VK_NULL_HANDLE }
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
				m_WindowProperties.width,
				m_WindowProperties.height,
				m_WindowProperties.title.c_str(),
				nullptr, // not fullscreen
				nullptr // don't share resources with other windows (opengl only)
 		);

		// Create a vulkan instance
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
	}

	void Core::Cleanup() {
		for(auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(m_Device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if(m_Debug) {
			debug::DestroyDebugUtilsMessengerEXT(m_pInstance, m_pDebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_pInstance, m_Surface, nullptr);
		vkDestroyInstance(m_pInstance, nullptr);

		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	void Core::CreateInstance() {
		if(m_Debug && !Validation::CheckValidationLayerSupport()) {
			// Check if validation layers are supported
			throw std::runtime_error("Validation layer requested, but none available!");
		}

		// Optional: Info about the app
		// APPINFO
		VkApplicationInfo appInfo{}; // pNext gets initialized to nullptr
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_WindowProperties.title.c_str();
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
		if(m_Debug) {
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
		if(vkCreateInstance(&createInfo, nullptr, &m_pInstance) != VK_SUCCESS) {
			// Throw an rte if Instance creation failed
			throw std::runtime_error("Failed to create VK instance!\n");
		}

		if(m_Debug){ // If debug flag is specified, print the extensions
			PrintVulkanExtensions();
		}
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
		for(const auto& extension : extensions) {
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

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		// Initialize the debug messenger
		if(debug::CreateDebugUtilsMessengerEXT(m_pInstance, &createInfo, nullptr, &m_pDebugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	void Core::PickPhysicalDevice() {
		uint32_t deviceCount{0};
		vkEnumeratePhysicalDevices(m_pInstance, &deviceCount, nullptr);

		// If no device with vulkan support is found, throw error
		if(deviceCount <= 0) {
			throw std::runtime_error("No GPUs with Vulkan support found!");
		}

		// Array to hold devices
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_pInstance, &deviceCount, devices.data()); // fill the array

		for(const auto& device : devices) {
			if(IsDeviceSuitable(device)) {
				m_PhysicalDevice = device;
				break;
			}
		}

		// If no suitable GPU is found, also throw an error
		if(m_PhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find a suitable GPU");
		}

		// Print the chosen device
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
		if(m_Debug) std::cout << "Running on physical device: " << deviceProperties.deviceName << std::endl;
	}

	bool Core::IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices{ FindQueueFamilies(device) };

		bool extensionSupported{ CheckDeviceExtensionSupport(device) };

		bool swapChainAdequate{ false };
		if(extensionSupported) {
			SwapChainSupportDetails swapChainSupport{ QuerySwapChainSupport(device) };
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.IsComplete() && extensionSupported && swapChainAdequate;

		// Could be extended to fall back on iGPU
		// Or select based on features
	}

	QueueFamilyIndices Core::FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount{0};
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		// Find a queue family that supports VK_QUEUE_GRAPHICS_BIT
		int i{0};
		for(const auto& queueFamily : queueFamilies) {
			if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport{false};
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

			if(presentSupport) {
				indices.presentFamily = i;
			}

			if(indices.IsComplete()) {
				break;
			}

			++i;
		}

		return indices;
	}

	bool Core::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(Validation::m_DeviceExtensions.begin(), Validation::m_DeviceExtensions.end());

		for(const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void Core::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

		// Fill the queueCreateInfo struct
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		// Set the priority of the queue
		float queuePriority = 1.0f;
		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}


		// Fill the deviceFeatures struct
		VkPhysicalDeviceFeatures deviceFeatures{}; // No special features required

		// Finally, fill the createInfo struct
		VkDeviceCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data(); // Reference the queue create info struct
		createInfo.pEnabledFeatures = &deviceFeatures; // Reference the device features struct
		// Depricated but a good idea to add anyway
		createInfo.enabledExtensionCount = static_cast<uint32_t>(Validation::m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = Validation::m_DeviceExtensions.data();
		if(m_Debug) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(Validation::m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = Validation::m_ValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		// Create the device
		if(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logic device!");
		}

		// Store the graphics queue
		vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue); // Only create single queue (0)
		// Store the presentation queue
		vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue); // idem
	}

	void Core::CreateSurface() {
		if(glfwCreateWindowSurface(m_pInstance, m_pWindow, nullptr, &m_Surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	SwapChainSupportDetails Core::QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		// Query basic surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

		// Query the supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
		if(formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
		}

		// Query the supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);
		if(presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR Core::ChooseSwapSurfaceFormats(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for(const auto& availableFormat : availableFormats) {
			if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		// If the format fails the flags, we could rank them based on how good they are, but in this case we just return the first one
		return availableFormats[0];
	}

	VkPresentModeKHR Core::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		// Prefer triple buffering
		for(const auto& availablePresentMode : availablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		// If mailbox is not available, use fifo
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Core::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		// return the current extend if width is set to special value (when using some window managers)
		if(capabilities.currentExtent.width != UINT32_MAX) {
			if(m_Debug) std::cout << "Window manager dictated special size.\n";
			return capabilities.currentExtent;
		}
		// else, calculate the size
		else {
			int width, height;
			glfwGetFramebufferSize(m_pWindow, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			// clamp the width and height between the minimum and maximum allowed width and height
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void Core::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormats(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		// Request one more image than minimum to avoid driver overhead
		uint32_t imageCount{ swapChainSupport.capabilities.minImageCount + 1 };
		if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// Fill the struct
		VkSwapchainCreateInfoKHR createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1; // Always 1 unless VR
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Can be set to VK_IMAGE_USAGE_TRANSFER_DST_BIT when eg. implementing post-processing

		QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
		uint32_t queueFamilyIndices[] {indices.graphicsFamily.value(), indices.presentFamily.value()};

		// If graphicsFamily & presentFamily differ, images must be explicitly transferred
		if(indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// Specify a transform
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

		// Specify if the alpha channel should be used for blending with other windows
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE; // Implemented later

		// Create the swapchain object
		if(vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void Core::CreateImageViews() {
		// Resize to fit all images
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		// Populate the image views
		for(size_t i{}; i < m_SwapChainImages.size(); ++i) {
			// Fill the struct
			VkImageViewCreateInfo createInfo{};

			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;

			// Components
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// Sub resource range
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// Create the image view
			if(vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create image views!");
			}
		}
	}

	void Core::CreateGraphicsPipeline() {

	}
}
