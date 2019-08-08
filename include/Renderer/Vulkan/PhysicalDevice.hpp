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
   public:
    PhysicalDevice(Instance& instance, Surface& surface);

    const VkPhysicalDevice& handle() const {
        return _device;
    }
};
}

#endif  // VULKANENGINE_PHYSICALDEVICE_HPP
