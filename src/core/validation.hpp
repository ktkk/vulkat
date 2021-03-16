#ifndef VALIDATION_HPP
#define VALIDATION_HPP

namespace vulkat {
	class Validation final {
	public:
		bool CheckValidationLayerSupport();
	
	private:
		const std::vector<const char*> m_ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
	};
}
#endif // VALIDATION_HPP
