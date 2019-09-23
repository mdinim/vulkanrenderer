//
// Created by Dániel Molnár on 2019-08-10.
//

#pragma once
#ifndef VULKANENGINE_RENDERPASS_HPP
#define VULKANENGINE_RENDERPASS_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/Images.hpp>

// ----- forward-decl -----
namespace Vulkan {
class Swapchain;
class LogicalDevice;
}  // namespace Vulkan
namespace Core {
class FileManager;
}

namespace Vulkan {
class RenderPass {
   private:
    const Swapchain& _swapchain;

    VkRenderPass _render_pass;

    std::unique_ptr<Image> _depth_image;
    std::unique_ptr<ImageView> _depth_image_view;

   public:
    RenderPass(const Swapchain& swapchain);
    ~RenderPass();

    ImageView& depth_image_view() const { return *_depth_image_view; }

    [[nodiscard]] const VkRenderPass& handle() const { return _render_pass; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_RENDERPASS_HPP
