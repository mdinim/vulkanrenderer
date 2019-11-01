//
// Created by Dániel Molnár on 2019-09-28.
//

#pragma once
#ifndef VULKANENGINE_IPIPELINE_HPP
#define VULKANENGINE_IPIPELINE_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward decl -----

namespace Vulkan {
class IPipeline {
   public:
    using VertexBindingDescContainer =
        std::vector<VkVertexInputBindingDescription>;
    using VertexAttribDescContainer =
        std::vector<VkVertexInputAttributeDescription>;
    using PushConstantContainer =
        std::vector<VkPushConstantRange>;

    [[nodiscard]] virtual const VkPipeline& handle() const = 0;
    [[nodiscard]] virtual const VkPipelineLayout& pipeline_layout() const = 0;
    virtual void recreate() = 0;

    virtual ~IPipeline() = default;
};
}  // namespace Vulkan
#endif  // VULKANENGINE_IPIPELINE_HPP
