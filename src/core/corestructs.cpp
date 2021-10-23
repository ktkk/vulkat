#include "../pch.hpp"
#include "corestructs.hpp"

namespace vulkat{
	Window::Window(const std::string& title, float width, float height, bool isVsyncOn)
		: title{ title }
		, width{ width }
		, height{ height }
		, isVsyncOn{ isVsyncOn }
	{}

	VkVertexInputBindingDescription Vertex::GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescription() {
		// 2 attributes are needed. One for position and one for color
		std::array<VkVertexInputAttributeDescription, 2> attributeDescription{};

		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT; // 2D 32bit signed floats (pos)
		attributeDescription[0].offset = offsetof(Vertex, pos);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT; //3D 32bit signed floats (color)
		attributeDescription[1].offset = offsetof(Vertex, color);

		return attributeDescription;
	}

	bool QueueFamilyIndices::IsComplete() {
		return graphicsFamily.has_value();
	}
}
