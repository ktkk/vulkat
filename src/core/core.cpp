#include <vector>

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

		CreateInstance();
	}

	void Core::CreateInstance() {
		// Optional: Info about the app
		VkApplicationInfo appInfo{}; // pNext gets initialized to nullptr
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_Window.title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = ENGINE;
		appInfo.engineVersion = VK_MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Not Optional: Vulkan extensions and validation layers
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;

		// Create the instance
		if (vkCreateInstance(&createInfo, nullptr, &m_pInstance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create VK instance!\n");
		}

		TestVulkanExtensions();
	}

	void Core::TestVulkanExtensions() const {
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

	void Core::Cleanup() {
		vkDestroyInstance(m_pInstance, nullptr);

		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}
}
