//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_UTILS_HPP
#define VULKANENGINE_UTILS_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies

// ----- forward decl -----

namespace Vulkan::Utils {
struct QueueFamily {
    std::optional<unsigned int> graphics_family;
    std::optional<unsigned int> present_family;

    explicit operator bool() const { return graphics_family && present_family; }
};

QueueFamily FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
}  // namespace Vulkan::Utils

#endif  // VULKANENGINE_UTILS_HPP
