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
    VkBufferUsageFlags _usage;

   public:
    Buffer(LogicalDevice& logical_device, VkDeviceSize buffer_size,
           VkBufferUsageFlags usage, VkSharingMode sharing_mode,
           VkMemoryPropertyFlags properties);

    virtual ~Buffer();

    [[nodiscard]] const VkBuffer& handle() const { return _buffer; }
    [[nodiscard]] const VkDeviceSize& size() const { return _size; }
    bool has_usage(VkBufferUsageFlagBits use) const { return _usage & use; }

    void transfer(void* data, unsigned int size,
                  unsigned int target_offset = 0);
};

class VertexBuffer : public Buffer {
   public:
    VertexBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~VertexBuffer() override = default;
};

class StagingBuffer : public Buffer {
   public:
    StagingBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~StagingBuffer() override = default;
};

class IndexBuffer : public Buffer {
   public:
    IndexBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~IndexBuffer() override = default;
};

class CombinedBuffer : public Buffer {
   public:
    CombinedBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~CombinedBuffer() override = default;
};

class UniformBuffer : public Buffer {
   public:
    UniformBuffer(LogicalDevice& logical_device, VkDeviceSize buffer_size);

    ~UniformBuffer() override = default;
};

}  // namespace Vulkan

#endif  // VULKANENGINE_BUFFERS_HPP
