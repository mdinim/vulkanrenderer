//
// Created by Dániel Molnár on 2019-09-22.
//

#pragma once
#ifndef VULKANENGINE_IMAGES_HPP
#define VULKANENGINE_IMAGES_HPP

// ----- std -----
#include <memory>

// ----- libraries -----
#include <vulkan/vulkan.h>
#include "ImageView.hpp"

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
namespace Memory {
class Block;
}
}  // namespace Vulkan

namespace Vulkan {
class Image {
   private:
    LogicalDevice& _logical_device;

    VkImage _image = VK_NULL_HANDLE;
    const Memory::Block* _block = nullptr;

    VkExtent3D _extent;
    unsigned int _mip_levels;
    unsigned int _array_layers;
    VkFormat _format;
    VkImageTiling _tiling;

    VkImageLayout _layout;
    VkImageUsageFlags _usage = 0;
    VkSharingMode _sharing_mode;

    VkMemoryPropertyFlags _properties;

   public:
    Image(LogicalDevice& logical_device, VkImageType type, unsigned int width,
          unsigned int height, unsigned int depth, unsigned int mip_levels,
          unsigned int array_layers, VkFormat format, VkImageTiling tiling,
          VkImageLayout initial_layout, VkImageUsageFlags usage,
          VkSharingMode sharing_mode, VkSampleCountFlagBits samples,
          VkImageCreateFlags flags, VkMemoryPropertyFlags properties);

    virtual ~Image();

    [[nodiscard]] const VkImage& handle() const { return _image; }

    [[nodiscard]] VkExtent3D extent() const { return _extent; }
    [[nodiscard]] unsigned int mip_levels() const { return _mip_levels; }
    [[nodiscard]] unsigned int array_layers() const { return _array_layers; }
    [[nodiscard]] VkFormat format() const { return _format; }

    void transition_layout(VkCommandBuffer command_buffer,
                           VkImageLayout new_layout);

    [[nodiscard]] std::unique_ptr<ImageView> create_view() const;
};

class Texture2D : public Image {
   public:
    Texture2D(LogicalDevice& logical_device, unsigned int width,
              unsigned int height);

    ~Texture2D() override = default;
};

}  // namespace Vulkan

#endif  // VULKANENGINE_IMAGES_HPP
