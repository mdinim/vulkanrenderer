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
    const LogicalDevice& _logical_device;

    VkRenderPass _render_pass;

   public:
    RenderPass(const LogicalDevice& logical_device, const Swapchain& swapchain);
    ~RenderPass();

    [[nodiscard]] const VkRenderPass& handle() const { return _render_pass; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_RENDERPASS_HPP
