//
// Created by Dániel Molnár on 2019-08-13.
//

#pragma once
#ifndef VULKANENGINE_BUFFERS_HPP
#define VULKANENGINE_BUFFERS_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class PhysicalDevice;
namespace Memory {
class Block;
}
}  // namespace Vulkan

namespace Vulkan {
class Buffer {
   private:
    LogicalDevice& _logical_device;

    VkBuffer _buffer;
    const Memory::Block* _block;
    VkDeviceSize _size;
    // VkDeviceMemory _memory;

   public:
    Buffer(LogicalDevice& logical_device, VkDeviceSize buffer_size,
           VkBufferUsageFlags usage, VkSharingMode sharing_mode,
           VkMemoryPropertyFlags properties);

    virtual ~Buffer();

    [[nodiscard]] const VkBuffer& handle() const { return _buffer; }
    [[nodiscard]] VkDeviceMemory memory() const;
    [[nodiscard]] VkDeviceSize offset() const;
    [[nodiscard]] const VkDeviceSize& size() const { return _size; }
};

class VertexBuffer : public Buffer {
   public:
    VertexBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~VertexBuffer() override = default;
};

class StagingBuffer : public Buffer {
   public:
    StagingBuffer(LogicalDevice& logicalDevice, VkDeviceSize bufferSize);

    ~StagingBuffer() override = default;
};

}  // namespace Vulkan

#endif  // VULKANENGINE_BUFFERS_HPP
