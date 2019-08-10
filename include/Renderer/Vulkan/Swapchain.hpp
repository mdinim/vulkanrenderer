//
// Created by Dániel Molnár on 2019-08-09.
//

#pragma once
#ifndef VULKANENGINE_SWAPCHAIN_HPP
#define VULKANENGINE_SWAPCHAIN_HPP

// ----- std -----
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/ImageView.hpp>

// ----- forward-decl -----

namespace Vulkan {
class PhysicalDevice;
class Surface;
class LogicalDevice;
}  // namespace Vulkan

namespace Vulkan {
class Swapchain {
   private:
    const Surface& _surface;
    const PhysicalDevice& _physical_device;
    const LogicalDevice& _logical_device;

    VkSwapchainKHR _swapchain;
    VkExtent2D _extent;
    VkFormat _format;

    std::vector<VkImage> _images;
    std::vector<ImageView> _image_views;

    void create();
    void teardown();
   public:
    Swapchain(const Surface& surface, const PhysicalDevice& physical_device,
              const LogicalDevice& logical_device);
    ~Swapchain();

    void recreate();

    [[nodiscard]] const VkSwapchainKHR& handle() const { return _swapchain; }
    [[nodiscard]] const VkExtent2D& extent() const { return _extent; }
    [[nodiscard]] const VkFormat& format() const { return _format; }
    const std::vector<VkImage>& images() const { return _images; }
    const std::vector<ImageView>& image_views() const { return _image_views; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SWAPCHAIN_HPP
