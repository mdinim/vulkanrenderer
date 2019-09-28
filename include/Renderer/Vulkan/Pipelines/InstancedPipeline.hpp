//
// Created by Dániel Molnár on 2019-09-28.
//

#pragma once
#ifndef VULKANENGINE_INSTANCEDPIPELINE_HPP
#define VULKANENGINE_INSTANCEDPIPELINE_HPP

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Pipelines/Pipeline.hpp>

// ----- forward-decl -----

namespace Vulkan {
class InstancedPipeline : public Pipeline<InstancedPipeline> {
   public:
    static const IPipeline::VertexBindingDescContainer& BindingDescriptions();
    static const IPipeline::VertexAttribDescContainer& AttributeDescriptions();

    InstancedPipeline(const Swapchain& swapchain)
        : Pipeline(swapchain, "instanced_shader_vert.spv", "shader_frag.spv") {}

    virtual ~InstancedPipeline() = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_INSTANCEDPIPELINE_HPP
