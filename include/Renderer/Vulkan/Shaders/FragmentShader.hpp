//
// Created by Dániel Molnár on 2019-09-30.
//

#pragma once
#ifndef VULKANENGINE_FRAGMENTSHADER_HPP
#define VULKANENGINE_FRAGMENTSHADER_HPP

// ----- std -----
#include <string>

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Shaders/ShaderBase.hpp>

// ----- forward-decl -----

namespace Vulkan {
class FragmentShader : public ShaderBase {
   public:
    FragmentShader(LogicalDevice& logical_device, std::string file_name,
                   std::string entry_point)
        : ShaderBase(logical_device, std::move(file_name),
                     std::move(entry_point)) {}

    ~FragmentShader() override = default;

    [[nodiscard]] VkShaderStageFlagBits stage() const override {
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_FRAGMENTSHADER_HPP
