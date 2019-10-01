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

    explicit InstancedPipeline(const Swapchain& swapchain)
        : Pipeline(swapchain) {}

    static std::vector<std::unique_ptr<IShader>> Shaders(
        LogicalDevice& logical_device) {
        std::vector<std::unique_ptr<IShader>> result;
        result.emplace_back(std::make_unique<VertexShader>(
                    logical_device, "instanced_shader_vert.spv", "main"));
        result.emplace_back(std::make_unique<FragmentShader>(
            logical_device, "shader_frag.spv", "main"));

        return result;
    }

    virtual ~InstancedPipeline() = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_INSTANCEDPIPELINE_HPP
