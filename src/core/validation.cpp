#include "../pch.hpp"
#include <cstring>
#include "validation.hpp"

namespace vulkat {
	const std::vector<const char*> Validation::m_ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> Validation::m_DeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	bool Validation::CheckValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_ValidationLayers) {
			bool layerFound{false};

			// Loop over the layers and check if they're available
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			// If any of the layers aren't found, return false
			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
}
