//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_UTILS_HPP
#define VULKANENGINE_UTILS_HPP

// ----- std -----
#include <optional>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

#include <Core/FileManager/BinaryFile.hpp>

// ----- in-project dependencies

// ----- forward decl -----

namespace Vulkan {
class PhysicalDevice;
class LogicalDevice;
class Surface;
}  // namespace Vulkan

namespace Vulkan::Utils {
struct QueueFamily {
    std::optional<unsigned int> graphics_family;
    std::optional<unsigned int> present_family;

    explicit operator bool() const { return graphics_family && present_family; }
};

uint32_t FindMemoryType(const PhysicalDevice& physical_device,
                        uint32_t type_filter_mask, VkMemoryPropertyFlags flags);

uint32_t FindMemoryType(VkPhysicalDevice physical_device,
                        uint32_t type_filter_mask, VkMemoryPropertyFlags flags);

QueueFamily FindQueueFamilies(const PhysicalDevice& device,
                              const Surface& surface);

QueueFamily FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

VkFormat FindSupportedFormat(const PhysicalDevice& physical_device,
                             const std::vector<VkFormat>& candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

VkFormat FindSupportedFormat(VkPhysicalDevice physical_device,
                             const std::vector<VkFormat>& candidates,
                             VkImageTiling tiling,
                             VkFormatFeatureFlags features);

VkFormat FindDepthFormat(const PhysicalDevice& physical_device);

VkFormat FindDepthFormat(VkPhysicalDevice physical_device);

bool HasStencilFormat(VkFormat format);

VkShaderModule CreateShaderModule(const Vulkan::LogicalDevice& logical_device,
                                  const Core::BinaryFile::ByteSequence& code);
}  // namespace Vulkan::Utils

#endif  // VULKANENGINE_UTILS_HPP
