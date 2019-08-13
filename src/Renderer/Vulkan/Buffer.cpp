//
// Created by Dániel Molnár on 2019-08-13.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffer.hpp>

// ----- std -----
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Utils.hpp>

namespace Vulkan {
Buffer::Buffer(const PhysicalDevice& physical_device,
               const LogicalDevice& logical_device, VkDeviceSize buffer_size,
               VkBufferUsageFlags usage, VkSharingMode sharing_mode)
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

    alloc_info.memoryTypeIndex =
        Utils::FindMemoryType(physical_device, mem_req.memoryTypeBits,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    alloc_info.allocationSize = mem_req.size;

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
}  // namespace Vulkan
