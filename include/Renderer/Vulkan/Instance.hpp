//
// Created by Dániel Molnár on 2019-08-07.
//

#pragma once
#ifndef VULKANENGINE_INSTANCE_HPP
#define VULKANENGINE_INSTANCE_HPP

// ----- std -----
#include <array>
#include <string>

// ----- libraries -----
#include <vulkan/vulkan.h>

// ----- in-project dependencies -----

// ----- forward decl -----
class IWindowService;

namespace Vulkan {
class Instance {
   private:
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;

    static constexpr const std::array<const char*, 1> _validation_layers = {
        "VK_LAYER_KHRONOS_validation"};

    static VKAPI_ATTR VkBool32 VKAPI_CALL validation_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data
    );

   public:
    static constexpr int Major = 0;
    static constexpr int Minor = 0;
    static constexpr int Patch = 1;

    Instance(const IWindowService& service, std::string application_name,
             std::string engine_name);
    ~Instance();

    void enumerate_extensions() const;

    const VkInstance& handle() const {
        return _instance;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_INSTANCE_HPP
