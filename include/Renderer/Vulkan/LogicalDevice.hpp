//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_LOGICALDEVICE_HPP
#define VULKANENGINE_LOGICALDEVICE_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan.h>

// ----- in-project dependencies -----

// ----- forward decl -----
namespace Vulkan {
class PhysicalDevice;
class Surface;
}

namespace Vulkan {
class LogicalDevice {
   private:
    VkDevice _device;
    VkQueue _graphics_queue;
    VkQueue _present_queue;

   public:
    LogicalDevice(PhysicalDevice& physicalDevice, Surface& surface);
    virtual ~LogicalDevice();

    VkDevice handle() const {
        return _device;
    }

    VkQueue graphics_queue_handle() const {
        return _graphics_queue;
    }

    VkQueue present_queue_handle() const {
        return _present_queue;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_LOGICALDEVICE_HPP