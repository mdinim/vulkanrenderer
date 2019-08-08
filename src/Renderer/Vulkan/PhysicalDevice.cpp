//
// Created by Dániel Molnár on 2019-08-08.
//

// ----- own header -----
#include <Renderer/Vulkan/PhysicalDevice.hpp>

// ----- std -----
#include <optional>
#include <vector>
#include <map>
#include <set>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Renderer.hpp>

namespace {
struct QueueFamily {
    std::optional<unsigned int> graphics_family;
    std::optional<unsigned int> present_family;

    explicit operator bool() const { return graphics_family && present_family; }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

QueueFamily FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    static std::map<std::pair<VkPhysicalDevice, VkSurfaceKHR>, QueueFamily>
        queue_family_cache;

    // early bail
    if (queue_family_cache[{device, surface}])
        return queue_family_cache.at({device, surface});

    auto& queue_family = queue_family_cache[{device, surface}];
    auto queue_family_count = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families.data());
    {
        VkBool32 present_supported = false;
        auto i = 0;
        for (const auto& found_family : queue_families) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                 &present_supported);
            if (found_family.queueCount > 0 &&
                (found_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                queue_family.graphics_family = i;
            }
            if (found_family.queueCount > 0 && present_supported) {
                queue_family.present_family = i;
            }

            if (queue_family) break;

            i++;
        }
    }

    return queue_family;
}

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

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    unsigned int format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         nullptr);
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         details.formats.data());

    unsigned int present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count,
                                         nullptr);
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count,
                                         details.present_modes.data());

    return details;
}

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    const auto SwapChainDetails = QuerySwapChainSupport(device, surface);

    const bool SwapChainAdequate = !SwapChainDetails.formats.empty() &&
                                   !SwapChainDetails.present_modes.empty();

    return static_cast<bool>(FindQueueFamilies(device, surface)) &&
           CheckExtensionSupport(device) && SwapChainAdequate;
}

int RateDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);

    if (!IsDeviceSuitable(device, surface)) return 0;

    auto queue_family = FindQueueFamilies(device, surface);

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
