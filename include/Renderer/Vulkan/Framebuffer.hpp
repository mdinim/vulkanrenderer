//
// Created by Dániel Molnár on 2019-08-12.
//

#pragma once
#ifndef VULKANENGINE_FRAMEBUFFER_HPP
#define VULKANENGINE_FRAMEBUFFER_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class Swapchain;
class ImageView;
}  // namespace Vulkan

namespace Vulkan {
class Framebuffer {
   private:
    const ImageView& _image_view;
    const Swapchain& _swapchain;

    VkFramebuffer _framebuffer;

   public:
    Framebuffer(const ImageView& image_view, const Swapchain& swapchain);

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&&);
    // We don't want after-construction move (swapchains might differ)
    Framebuffer& operator=(Framebuffer&&) = delete;

    ~Framebuffer();

    [[nodiscard]] const ImageView& image_view() const { return _image_view; }
    [[nodiscard]] const VkFramebuffer& handle() const { return _framebuffer; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_FRAMEBUFFER_HPP
