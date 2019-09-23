//
// Created by Dániel Molnár on 2019-08-08.
//

// ----- own header -----
#include <Renderer/Vulkan/PhysicalDevice.hpp>

// ----- std -----
#include <map>
#include <optional>
#include <set>
#include <vector>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Renderer.hpp>
#include <Renderer/Vulkan/Utils.hpp>

namespace {
bool CheckExtensionSupport(VkPhysicalDevice device) {
    unsigned int extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                         nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);

    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                         available_extensions.data());

    std::set<std::string> remaining_extensions(
        Vulkan::Renderer::RequiredExtensions.begin(),
        Vulkan::Renderer::RequiredExtensions.end());

    for (const auto& extension : available_extensions) {
        if (remaining_extensions.find(extension.extensionName) !=
            remaining_extensions.end()) {
            remaining_extensions.erase(extension.extensionName);
        }
    }

    return remaining_extensions.empty();
}

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface,
                      const VkPhysicalDeviceFeatures& supported_features) {
    const auto SwapChainDetails =
        Vulkan::Utils::QuerySwapChainSupport(device, surface);

    const bool SwapChainAdequate = !SwapChainDetails.formats.empty() &&
                                   !SwapChainDetails.present_modes.empty();

    return static_cast<bool>(
               Vulkan::Utils::FindQueueFamilies(device, surface)) &&
           CheckExtensionSupport(device) && SwapChainAdequate &&
           supported_features.samplerAnisotropy; // TODO may be optional
}

int RateDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &supported_features);

    if (!IsDeviceSuitable(device, surface, supported_features)) return 0;

    auto queue_family = Vulkan::Utils::FindQueueFamilies(device, surface);

    int score = 0;

    if (queue_family.present_family == queue_family.graphics_family)
        score += 10;

    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    score += device_properties.limits.maxImageDimension2D;
    score += device_properties.limits.maxImageDimension3D;

    return score;
}
}  // namespace

namespace Vulkan {
Vulkan::PhysicalDevice::PhysicalDevice(Vulkan::Instance& instance,
                                       Vulkan::Surface& surface) {
    // Select physical device
    auto device_count = 0u;
    vkEnumeratePhysicalDevices(instance.handle(), &device_count, nullptr);

    if (device_count == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance.handle(), &device_count,
                               devices.data());

    int best_score = 0;
    for (const auto& device : devices) {
        auto score = RateDevice(device, surface.handle());

        // unsuitable
        if (score == 0) continue;

        if (score > best_score) {
            _device = device;
        }
    }

    if (_device == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find suitable GPU");
}
}  // namespace Vulkan
