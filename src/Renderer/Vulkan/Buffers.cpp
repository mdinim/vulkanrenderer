//
// Created by Dániel Molnár on 2019-08-13.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- std -----
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Utils.hpp>

namespace Vulkan {

// ------ BUFFER -------

Buffer::Buffer(const PhysicalDevice& physical_device,
               const LogicalDevice& logical_device, VkDeviceSize buffer_size,
               VkBufferUsageFlags usage, VkSharingMode sharing_mode,
               VkMemoryPropertyFlags properties)
    : _logical_device(logical_device), _size(buffer_size) {
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    create_info.size = buffer_size;
    create_info.usage = usage;
    create_info.sharingMode = sharing_mode;

    if (vkCreateBuffer(logical_device.handle(), &create_info, nullptr,
                       &_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Could not create buffer");
    }

    VkMemoryRequirements mem_req;
    vkGetBufferMemoryRequirements(_logical_device.handle(), _buffer, &mem_req);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    alloc_info.memoryTypeIndex = Utils::FindMemoryType(
        physical_device, mem_req.memoryTypeBits, properties);
    alloc_info.allocationSize = mem_req.size;

    // TODO introduce VMA
    if (vkAllocateMemory(_logical_device.handle(), &alloc_info, nullptr,
                         &_memory) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate memory for buffer");
    }

    vkBindBufferMemory(_logical_device.handle(), _buffer, _memory, 0);
}

Buffer::~Buffer() {
    vkFreeMemory(_logical_device.handle(), _memory, nullptr);
    vkDestroyBuffer(_logical_device.handle(), _buffer, nullptr);
}

// ------ VERTEX BUFFER -------

VertexBuffer::VertexBuffer(const Vulkan::PhysicalDevice& physical_device,
                           const Vulkan::LogicalDevice& logical_device,
                           VkDeviceSize buffer_size)
    : Buffer(
          physical_device, logical_device, buffer_size,
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

// ------ STAGING BUFFER -------

StagingBuffer::StagingBuffer(const PhysicalDevice& physicalDevice,
                             const LogicalDevice& logicalDevice,
                             VkDeviceSize bufferSize)
    : Buffer(physicalDevice, logicalDevice, bufferSize,
             VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {}
}  // namespace Vulkan
