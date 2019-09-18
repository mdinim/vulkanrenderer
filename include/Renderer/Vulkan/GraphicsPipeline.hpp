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

    const Swapchain& _swapchain;

    VkDescriptorSetLayout _descriptor_set_layout;
    VkPipelineLayout _pipeline_layout;
    VkPipeline _graphics_pipeline;

   public:
    GraphicsPipeline(const Swapchain& swapchain);
    ~GraphicsPipeline();

    [[nodiscard]] const VkPipeline& handle() const {
        return _graphics_pipeline;
    };

    //TODO remove
    VkDescriptorSetLayout descriptor_set_layout() const {
        return _descriptor_set_layout;
    }
    //TODO remove
    VkPipelineLayout pipeline_layout() const {
        return _pipeline_layout;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_GRAPHICSPIPELINE_HPP
