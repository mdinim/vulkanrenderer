//
// Created by Dániel Molnár on 2019-08-09.
//

#pragma once
#ifndef VULKANENGINE_IMAGEVIEW_HPP
#define VULKANENGINE_IMAGEVIEW_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
class Image;
}  // namespace Vulkan

namespace Vulkan {
class ImageView {
   private:
    const LogicalDevice& _logical_device;
    VkImageView _image_view;

   public:
    ImageView(const LogicalDevice& logical_device, const Image& image,
              VkImageAspectFlags aspect, const VkComponentMapping& mapping);

    ImageView(const ImageView&) = delete;
    ImageView& operator=(const ImageView&) = delete;

    ImageView(ImageView&&);
    // We don't want after-construction move (swapchains and logical devices
    // might differ, causing mixed state)
    ImageView& operator=(ImageView&&) = delete;

    ~ImageView();

    [[nodiscard]] const VkImageView& handle() const { return _image_view; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_IMAGEVIEW_HPP
