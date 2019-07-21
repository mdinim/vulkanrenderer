#ifndef _VULKANENGINE_CONFIGURATION_HPP_
#define _VULKANENGINE_CONFIGURATION_HPP_

namespace Configuration {
#if NDEBUG
constexpr const bool Debug = false;
#else
constexpr const bool Debug = true;
#endif
constexpr const bool EnableVulkanValidationLayers = Debug;
}  // namespace Configuration
#endif
