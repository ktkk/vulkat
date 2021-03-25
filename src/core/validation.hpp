#ifndef VALIDATION_HPP
#define VALIDATION_HPP

namespace vulkat {
	class Validation final {
	public:
		static bool CheckValidationLayerSupport();
		static const std::vector<const char*> m_ValidationLayers;
		static const std::vector<const char*> m_DeviceExtensions;
	};
}
#endif // VALIDATION_HPP
