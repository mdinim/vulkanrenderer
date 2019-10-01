//
// Created by Dániel Molnár on 2019-09-30.
//

// ----- own header -----
#include <Renderer/Vulkan/Shaders/ShaderBase.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <directories.hpp>

namespace {
VkShaderModule CreateShaderModule(const Vulkan::LogicalDevice& logical_device,
                                  const Core::BinaryFile::ByteSequence& code) {
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(logical_device.handle(), &create_info, nullptr,
                             &module) != VK_SUCCESS) {
        throw std::runtime_error("Could not creat shader module!");
    }

    return module;
}
}  // namespace

namespace Vulkan {
Core::FileManager& ShaderBase::ShaderManager() {
    static Core::FileManager shader_manager(
        {std::filesystem::current_path(), builtin_shader_dir});

    return shader_manager;
}

ShaderBase::ShaderBase(LogicalDevice& logical_device, std::string file_name,
                       std::string entry_point)
    : _logical_device(logical_device),
      _file_name(std::move(file_name)),
      _entry_point(std::move(entry_point)) {
    if (auto shader_file = ShaderManager().binary_file(_file_name)) {
        if (auto shader_code = shader_file->read()) {
            _shader_module =
                CreateShaderModule(_logical_device, shader_code.value());
        } else {
            throw std::runtime_error("Shader file " +
                                     shader_file->path().string() +
                                     " could not be read");
        }
    } else {
        throw std::runtime_error("Built-in vertex shader not found");
    }
}

VkShaderModule ShaderBase::module() const { return _shader_module; }

const char* ShaderBase::entry_point() const { return _entry_point.c_str(); }

ShaderBase::~ShaderBase() {
    vkDestroyShaderModule(_logical_device.handle(), _shader_module, nullptr);
}
}