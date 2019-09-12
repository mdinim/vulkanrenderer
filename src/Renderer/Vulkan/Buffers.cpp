//
// Created by Dániel Molnár on 2019-08-13.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- std -----
#include <iostream>  // TODO Remove
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Allocator.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Utils.hpp>

namespace Vulkan {

// ------ BUFFER -------

Buffer::Buffer(LogicalDevice& logical_device, VkDeviceSize buffer_size,
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

    _block = &_logical_device.request_memory(mem_req, properties);

    /*VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    alloc_info.memoryTypeIndex = Utils::FindMemoryType(
        physical_device, mem_req.memoryTypeBits, properties);
    alloc_info.allocationSize = mem_req.size;

    // TODO introduce VMA
    if (vkAllocateMemory(_logical_device.handle(), &alloc_info, nullptr,
                         &_memory) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate memory for buffer");
    }*/

    // vkBindBufferMemory(_logical_device.handle(), _buffer, _memory, 0);

    vkBindBufferMemory(_logical_device.handle(), _buffer, _block->memory(),
                       _block->offset().value);
}

Buffer::~Buffer() {
    vkDestroyBuffer(_logical_device.handle(), _buffer, nullptr);
    // vkFreeMemory(_logical_device.handle(), _memory, nullptr);
}

VkDeviceMemory Buffer::memory() const {
    // return _memory;
    return _block->memory();
}

VkDeviceSize Buffer::offset() const {
    // return 0;
    return _block->offset().value;
}

// ------ VERTEX BUFFER -------

VertexBuffer::VertexBuffer(LogicalDevice& logical_device,
                           VkDeviceSize buffer_size)
    : Buffer(
          logical_device, buffer_size,
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

// ------ STAGING BUFFER -------

StagingBuffer::StagingBuffer(LogicalDevice& logicalDevice,
                             VkDeviceSize bufferSize)
    : Buffer(logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
             VK_SHARING_MODE_EXCLUSIVE,
             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {}
}  // namespace Vulkan
