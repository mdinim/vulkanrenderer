#ifndef _VULKANENGINE_CONFIGURATION_HPP_
#define _VULKANENGINE_CONFIGURATION_HPP_

namespace Configuration {
#if NDEBUG
constexpr const bool Debug = false;
#else
constexpr const bool Debug = true;
#endif
constexpr const bool EnableVulkanValidationLayers = Debug;
constexpr const unsigned int CommandPoolBatchSize = 2;

static_assert(CommandPoolBatchSize > 0,
              "Command pool factor should be a positive number");
}  // namespace Configuration
#endif
