//
// Created by Dániel Molnár on 2019-08-13.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- std -----
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Images.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Memory/Allocator.hpp>
#include <Renderer/Vulkan/Utils.hpp>

namespace Vulkan {

// ------ BUFFER -------

Buffer::Buffer(LogicalDevice& logical_device, VkDeviceSize buffer_size,
               VkBufferUsageFlags usage, VkSharingMode sharing_mode,
               VkMemoryPropertyFlags properties)
    : _logical_device(logical_device),
      _size(buffer_size),
      _usage(usage),
      _sharing_mode(sharing_mode),
      _properties(properties) {
    allocate();
}

Buffer::Buffer(Vulkan::LogicalDevice& logical_device,
               VkSharingMode sharing_mode, VkMemoryPropertyFlags properties)
    : _logical_device(logical_device),
      _sharing_mode(sharing_mode),
      _properties(properties) {}

Buffer::~Buffer() {
    vkDestroyBuffer(_logical_device.handle(), _buffer, nullptr);
    if (_block) _logical_device.release_memory(*_block);
}

void Buffer::allocate() {
    if (!_block) {
        if (!_buffer) {
            VkBufferCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

            create_info.size = _size;
            create_info.usage = _usage;
            create_info.sharingMode = _sharing_mode;

            if (vkCreateBuffer(_logical_device.handle(), &create_info, nullptr,
                               &_buffer) != VK_SUCCESS) {
                throw std::runtime_error("Could not create buffer");
            }
        }
        VkMemoryRequirements mem_req;
        vkGetBufferMemoryRequirements(_logical_device.handle(), _buffer,
                                      &mem_req);

        _block = &_logical_device.request_memory(mem_req, _properties);

        vkBindBufferMemory(_logical_device.handle(), _buffer, _block->memory(),
                           _block->offset().value);
    }
}

void Buffer::transfer(void* data, unsigned int size,
                      unsigned int target_offset) {
    if (!_block) throw std::runtime_error("Buffer has no memory allocated!");

    _block->transfer(data, size, target_offset);
}

void Buffer::transfer(void* data, const SubBufferDescriptor& desc) {
    transfer(data, desc.size, desc.offset);
}

void Buffer::copy_to(TempCommandBuffer& buffer, Buffer& dst,
                     const std::vector<SubBufferDescriptor>& src_descs,
                     const std::vector<SubBufferDescriptor>& dst_descs) {
    if (!has_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
        !dst.has_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT) ||
        src_descs.size() != dst_descs.size())
        throw std::runtime_error("Can not execute buffer data copy!");

    std::vector<VkBufferCopy> copy_regions;
    for (auto i = 0ul; i < dst_descs.size(); ++i) {
        VkBufferCopy copy_region = {};
        copy_region.srcOffset = src_descs.at(i).offset;
        copy_region.dstOffset = dst_descs.at(i).offset;
        copy_region.size = src_descs.at(i).size;

        copy_regions.push_back(copy_region);
    }
    vkCmdCopyBuffer(buffer.handle(), handle(), dst.handle(),
                    copy_regions.size(), copy_regions.data());
}

void Buffer::copy_to(TempCommandBuffer& buffer,
                     const SubBufferDescriptor& src_desc, Image& dst) {
    if (!has_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        throw std::runtime_error("Can not execute buffer data copy");
    }

    dst.transition_layout(buffer.handle(),
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy buffer_image_copy = {};
    buffer_image_copy.imageExtent = dst.extent();
    buffer_image_copy.bufferOffset = src_desc.offset;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.bufferRowLength = 0;

    buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = dst.array_layers();

    vkCmdCopyBufferToImage(buffer.handle(), handle(), dst.handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &buffer_image_copy);
}

// ------ VERTEX BUFFER -------

VertexBuffer::VertexBuffer(LogicalDevice& logical_device,
                           VkDeviceSize buffer_size)
    : Buffer(
          logical_device, buffer_size,
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
          VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

// ------ STAGING BUFFER -------

StagingBuffer::StagingBuffer(LogicalDevice& logical_device,
                             VkDeviceSize buffer_size)
    : Buffer(logical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
             VK_SHARING_MODE_EXCLUSIVE,
             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {}

IndexBuffer::IndexBuffer(LogicalDevice& logical_device,
                         VkDeviceSize buffer_size)
    : Buffer(
          logical_device, buffer_size,
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

CombinedBuffer::CombinedBuffer(LogicalDevice& logical_device,
                               VkDeviceSize buffer_size)
    : Buffer(logical_device, buffer_size,
             VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT,
             VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

UniformBuffer::UniformBuffer(LogicalDevice& logical_device,
                             VkDeviceSize buffer_size)
    : Buffer(logical_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
             VK_SHARING_MODE_EXCLUSIVE,
             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {}
}  // namespace Vulkan
