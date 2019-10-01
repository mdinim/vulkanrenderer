//
// Created by Dániel Molnár on 2019-09-30.
//

#pragma once
#ifndef VULKANENGINE_SHADERBASE_HPP
#define VULKANENGINE_SHADERBASE_HPP

// ----- std -----

// ----- libraries -----
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Shaders/IShader.hpp>

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}

namespace Vulkan {
class ShaderBase : public IShader {
   private:
    static Core::FileManager& ShaderManager();

    LogicalDevice& _logical_device;

    std::string _file_name;
    std::string _entry_point;

    VkShaderModule _shader_module = VK_NULL_HANDLE;

   public:
    ShaderBase(LogicalDevice& logical_device, std::string file_name,
               std::string entry_point);
    virtual ~ShaderBase();

    [[nodiscard]] VkShaderModule module() const override;

    [[nodiscard]] const char* entry_point() const override;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SHADERBASE_HPP
