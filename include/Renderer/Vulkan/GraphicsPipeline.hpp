//
// Created by Dániel Molnár on 2019-08-10.
//

#pragma once
#ifndef VULKANENGINE_GRAPHICSPIPELINE_HPP
#define VULKANENGINE_GRAPHICSPIPELINE_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----

// ----- forward decl -----
namespace Vulkan {
class LogicalDevice;
class Swapchain;
}  // namespace Vulkan

namespace Vulkan {
class GraphicsPipeline {
   private:
    Core::FileManager _shader_manager;

    const LogicalDevice& _logical_device;
    const Swapchain& _swapchain;

    VkPipelineLayout _pipeline_layout;
    VkPipeline _graphics_pipeline;

   public:
    GraphicsPipeline(const LogicalDevice& logical_device,
                     const Swapchain& swapchain);
    ~GraphicsPipeline();

    [[nodiscard]] const VkPipeline& handle() const {
        return _graphics_pipeline;
    };
};
}  // namespace Vulkan

#endif  // VULKANENGINE_GRAPHICSPIPELINE_HPP
