//
// Created by Dániel Molnár on 2019-09-30.
//

#pragma once
#ifndef VULKANENGINE_VERTEXSHADER_HPP
#define VULKANENGINE_VERTEXSHADER_HPP

// ----- std -----
#include <string>

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Shaders/ShaderBase.hpp>

// ----- forward-decl -----

namespace Vulkan {
class VertexShader : public ShaderBase {
   public:
    VertexShader(LogicalDevice& logical_device, std::string file_name,
                 std::string entry_point)
        : ShaderBase(logical_device, std::move(file_name),
                     std::move(entry_point)) {}

    ~VertexShader() override = default;

    [[nodiscard]] VkShaderStageFlagBits stage() const override {
        return VK_SHADER_STAGE_VERTEX_BIT;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_VERTEXSHADER_HPP
