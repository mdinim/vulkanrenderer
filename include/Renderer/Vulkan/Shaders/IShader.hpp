//
// Created by Dániel Molnár on 2019-09-30.
//

#pragma once
#ifndef VULKANENGINE_ISHADER_HPP
#define VULKANENGINE_ISHADER_HPP

// ----- std -----
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----

namespace Vulkan {
class IShader {
   public:
    [[nodiscard]] virtual VkShaderModule module() const = 0;
    [[nodiscard]] virtual VkShaderStageFlagBits stage() const = 0;
    [[nodiscard]] virtual const char* entry_point() const = 0;

    virtual ~IShader() = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_ISHADER_HPP
