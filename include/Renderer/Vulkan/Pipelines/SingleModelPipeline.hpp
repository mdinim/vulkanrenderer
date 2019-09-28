//
// Created by Dániel Molnár on 2019-09-28.
//

#pragma once
#ifndef VULKANENGINE_SINGLEMODELPIPELINE_HPP
#define VULKANENGINE_SINGLEMODELPIPELINE_HPP

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Pipelines/Pipeline.hpp>

// ----- forward-decl -----

namespace Vulkan {
class SingleModelPipeline : public Pipeline<SingleModelPipeline> {
   public:
    static const IPipeline::VertexBindingDescContainer& BindingDescriptions();
    static const IPipeline::VertexAttribDescContainer& AttributeDescriptions();

    explicit SingleModelPipeline(const Swapchain& swapchain)
        : Pipeline(swapchain, "shader_vert.spv", "shader_frag.spv") {}

    virtual ~SingleModelPipeline() = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SINGLEMODELPIPELINE_HPP
