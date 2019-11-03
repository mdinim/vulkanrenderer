//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_PHYSICALDEVICE_HPP
#define VULKANENGINE_PHYSICALDEVICE_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Instance.hpp>
#include <Renderer/Vulkan/Surface.hpp>

// ----- forward decl -----

namespace Vulkan {
class PhysicalDevice {
   private:
    VkPhysicalDevice _device = VK_NULL_HANDLE;

    VkPhysicalDeviceFeatures _features;

    VkPhysicalDeviceProperties _properties;
   public:
    PhysicalDevice(Instance& instance, Surface& surface);

    [[nodiscard]] const VkPhysicalDevice& handle() const {
        return _device;
    }

    [[nodiscard]] const VkPhysicalDeviceFeatures& features() const {
        return _features;
    }

    [[nodiscard]] const VkPhysicalDeviceProperties& properties() const {
        return _properties;
    }
};
}

#endif  // VULKANENGINE_PHYSICALDEVICE_HPP
