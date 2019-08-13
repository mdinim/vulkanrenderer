//
// Created by Dániel Molnár on 2019-08-13.
//

#pragma once
#ifndef VULKANENGINE_BUFFER_HPP
#define VULKANENGINE_BUFFER_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class PhysicalDevice;
}  // namespace Vulkan

namespace Vulkan {
class Buffer {
   private:
    const LogicalDevice& _logical_device;

    VkBuffer _buffer;
    VkDeviceMemory _memory;
    VkDeviceSize _size;

   public:
    Buffer(const PhysicalDevice& physical_device,
           const LogicalDevice& logical_device, VkDeviceSize buffer_size,
           VkBufferUsageFlags usage, VkSharingMode sharing_mode);

    virtual ~Buffer();

    [[nodiscard]] const VkBuffer& handle() const { return _buffer; }
    [[nodiscard]] const VkDeviceMemory& memory() const { return _memory; }
    [[nodiscard]] const VkDeviceSize& size() const { return _size; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_BUFFER_HPP
