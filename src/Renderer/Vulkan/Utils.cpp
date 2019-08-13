//
// Created by Dániel Molnár on 2019-08-08.
//

// ----- std -----
#include <map>
#include <vector>

// ----- own header -----
#include <Renderer/Vulkan/Utils.hpp>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Surface.hpp>

// ----- forward decl -----

namespace Vulkan::Utils {
uint32_t FindMemoryType(const PhysicalDevice& physical_device,
                        uint32_t type_filter_mask,
                        VkMemoryPropertyFlags flags) {
    return FindMemoryType(physical_device.handle(), type_filter_mask, flags);
}

uint32_t FindMemoryType(VkPhysicalDevice physical_device,
                        uint32_t type_filter_mask,
                        VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties properties = {};
    vkGetPhysicalDeviceMemoryProperties(physical_device, &properties);

    for (auto i = 0u; i < properties.memoryTypeCount; i++) {
        if (type_filter_mask & (1u << i) &&
            (properties.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;
        }
    }

    throw std::runtime_error("Suitable memory type not found!");
}

QueueFamily FindQueueFamilies(const PhysicalDevice& device,
                              const Surface& surface) {
    return FindQueueFamilies(device.handle(), surface.handle());
}

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
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &present_mode_count, nullptr);
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, details.present_modes.data());

    return details;
}
}