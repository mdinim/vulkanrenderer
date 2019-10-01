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

    explicit SingleModelPipeline(
        const Swapchain& swapchain,
        const std::vector<VkDescriptorSetLayout>& layouts)
        : Pipeline(swapchain, layouts) {}

    static std::vector<std::unique_ptr<IShader>> Shaders(
        LogicalDevice& logical_device) {
        std::vector<std::unique_ptr<IShader>> result;
        result.emplace_back(std::make_unique<VertexShader>(
            logical_device, "shader_vert.spv", "main"));
        result.emplace_back(std::make_unique<FragmentShader>(
            logical_device, "shader_frag.spv", "main"));

        return result;
    }

    ~SingleModelPipeline() override = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SINGLEMODELPIPELINE_HPP
