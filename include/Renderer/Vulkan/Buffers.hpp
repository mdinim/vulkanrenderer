//
// Created by Dániel Molnár on 2019-08-13.
//

#pragma once
#ifndef VULKANENGINE_BUFFERS_HPP
#define VULKANENGINE_BUFFERS_HPP

// ----- std -----
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>
#include <Core/Utils/Utils.hpp>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class PhysicalDevice;
class TempCommandBuffer;
class Image;
namespace Memory {
class Block;
}
}  // namespace Vulkan

namespace Vulkan {

struct SubBufferDescriptor {
    VkBufferUsageFlags usage;
    VkDeviceSize size;
    VkDeviceSize offset;
};

struct IndexBufferTag {
    static constexpr VkBufferUsageFlags Usage =
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    static constexpr VkSharingMode SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    static constexpr VkMemoryPropertyFlags MemoryProperties =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

struct VertexBufferTag {
    static constexpr VkBufferUsageFlags Usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    static constexpr VkSharingMode SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    static constexpr VkMemoryPropertyFlags MemoryProperties =
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
};

struct StagingBufferTag {
    static constexpr VkBufferUsageFlags Usage =
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    static constexpr VkSharingMode SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    static constexpr VkMemoryPropertyFlags MemoryProperties =
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
};

struct UniformBufferTag {
    static constexpr VkBufferUsageFlags Usage =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    static constexpr VkSharingMode SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    static constexpr VkMemoryPropertyFlags MemoryProperties =
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
};

class Buffer {
   protected:
    LogicalDevice& _logical_device;

    VkBuffer _buffer = VK_NULL_HANDLE;
    const Memory::Block* _block = nullptr;
    VkDeviceSize _size = 0;
    VkBufferUsageFlags _usage = 0;
    VkSharingMode _sharing_mode;
    VkMemoryPropertyFlags _properties;

    Buffer(LogicalDevice& logical_device, VkSharingMode sharing_mode,
           VkMemoryPropertyFlags properties);

   public:
    Buffer(LogicalDevice& logical_device, VkDeviceSize buffer_size,
           VkBufferUsageFlags usage, VkSharingMode sharing_mode,
           VkMemoryPropertyFlags properties);

    virtual ~Buffer();

    [[nodiscard]] const VkBuffer& handle() const { return _buffer; }
    [[nodiscard]] const VkDeviceSize& size() const { return _size; }
    [[nodiscard]] bool has_usage(VkBufferUsageFlagBits use) const {
        return _usage & use;
    }

    void allocate();
    void transfer(void* data, unsigned int size,
                  unsigned int target_offset = 0);
    void transfer(void* data, const SubBufferDescriptor& desc);

    void copy_to(TempCommandBuffer& buffer, Buffer& dst,
                 const std::vector<SubBufferDescriptor>& src_descs,
                 const std::vector<SubBufferDescriptor>& dst_descs);

    void copy_to(TempCommandBuffer& buffer, const SubBufferDescriptor& src_desc,
                 Image& dst);
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

template <class T, class Ts>
constexpr bool compatible_sub_buffers() {
    constexpr auto fn = [](const auto& x) {
        return T::MemoryProperties == x.MemoryProperties &&
               T::SharingMode == x.SharingMode;
    };
    return std::apply([&fn](const auto&... xs) { return (fn(xs) && ...); },
                      Ts());
}

template <class... SubBufferTags>
class PolymorphBuffer : public Buffer {
   private:
    using SubBuffers = std::tuple<SubBufferTags...>;
    using FirstSubBuffer = typename std::tuple_element<0, SubBuffers>::type;

    static_assert(compatible_sub_buffers<FirstSubBuffer, SubBuffers>(),
                  "Incompatible sub buffers!");

   public:
    explicit PolymorphBuffer(LogicalDevice& logical_device)
        : Buffer(logical_device, FirstSubBuffer::SharingMode,
                 FirstSubBuffer::MemoryProperties) {}

    template <class SubBufferTag>
    SubBufferDescriptor commit_sub_buffer(VkDeviceSize size) {
        static_assert(Core::contains<SubBufferTag, SubBufferTags...>(),
                      "Invalid sub buffer tag");

        SubBufferDescriptor descriptor{SubBufferTag::Usage, size, _size};
        _usage |= SubBufferTag::Usage;
        _size += size;

        return descriptor;
    }
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

class DynamicUniformBuffer : public Buffer {
   private:
    VkDeviceSize _dynamic_alignment;
    VkDeviceSize _element_size;
    size_t _element_count;
    size_t _next_slot = 0;

   public:
    DynamicUniformBuffer(const PhysicalDevice& physical_device,
                         LogicalDevice& logical_device,
                         VkDeviceSize element_size, size_t element_count);
    ~DynamicUniformBuffer() override = default;

    [[nodiscard]] SubBufferDescriptor acquire_slot();
};

}  // namespace Vulkan

#endif  // VULKANENGINE_BUFFERS_HPP
