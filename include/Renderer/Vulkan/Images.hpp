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
#include <Renderer/Vulkan/ImageView.hpp>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class Swapchain;
namespace Memory {
class Block;
}
}  // namespace Vulkan

namespace Vulkan {
class Image {
   protected:
    LogicalDevice& _logical_device;

    VkImage _image = VK_NULL_HANDLE;
    const Memory::Block* _block = nullptr;

    VkExtent3D _extent;
    unsigned int _mip_levels;
    unsigned int _array_layers;
    VkFormat _format;

    VkImageLayout _layout;
    VkImageUsageFlags _usage = 0;
    VkSharingMode _sharing_mode;

    VkMemoryPropertyFlags _properties;

    // To take a swapchain image and represent it internally the same way
    Image(LogicalDevice& logical_device, VkImage image, unsigned int width,
          unsigned int height, unsigned int depth, unsigned int mip_levels,
          unsigned int array_layers, VkFormat format,
          VkImageLayout initial_layout, VkImageUsageFlags usage,
          VkSharingMode sharing_mode, VkMemoryPropertyFlags properties);

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
    [[nodiscard]] VkImageLayout layout() const { return _layout; }

    virtual void transition_layout(VkCommandBuffer command_buffer,
                                   VkImageLayout new_layout);

    [[nodiscard]] virtual VkImageViewType view_type() const = 0;

    [[nodiscard]] std::unique_ptr<ImageView> create_view(
        VkImageAspectFlags aspect,
        const VkComponentMapping& mapping = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY}) const;
};

class SwapchainImage : public Image {
   public:
    SwapchainImage(const Swapchain& swapchain, VkImage image);

    ~SwapchainImage() override {
        _image = VK_NULL_HANDLE;  // swapchain images need no cleanup, just set
                                  // it to null handle
    }

    void transition_layout(VkCommandBuffer, VkImageLayout) override {
        throw std::runtime_error("Swapchain images can not be transitioned!");
    }

    [[nodiscard]] VkImageViewType view_type() const override {
        return VK_IMAGE_VIEW_TYPE_2D;
    }
};

class DepthImage : public Image {
   public:
    DepthImage(const Swapchain& swapchain, VkFormat depth_format);

    ~DepthImage() override = default;

    [[nodiscard]] VkImageViewType view_type() const override {
        return VK_IMAGE_VIEW_TYPE_2D;
    }
};

}  // namespace Vulkan

#endif  // VULKANENGINE_IMAGES_HPP
