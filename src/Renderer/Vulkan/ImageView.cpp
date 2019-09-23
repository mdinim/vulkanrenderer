//
// Created by Dániel Molnár on 2019-08-09.
//

// ----- own header -----
#include "Renderer/Vulkan/ImageView.hpp"

// ----- std -----
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>

// ----- forward-decl -----

namespace Vulkan {
ImageView::ImageView(const LogicalDevice& logical_device, const Image& image, VkImageAspectFlags aspect, const VkComponentMapping& mapping)
    : _logical_device(logical_device) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    create_info.image = image.handle();
    create_info.viewType = image.view_type();
    create_info.format = image.format();

    create_info.components = mapping;

    create_info.subresourceRange.aspectMask = aspect;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = image.array_layers();
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = image.mip_levels();

    if (vkCreateImageView(_logical_device.handle(), &create_info, nullptr,
                          &_image_view) != VK_SUCCESS) {
        throw std::runtime_error("Could not create image view");
    }
}

ImageView::ImageView(ImageView&& other)
    : _logical_device(other._logical_device) {
    _image_view = other._image_view;
    other._image_view = VK_NULL_HANDLE;
}

ImageView::~ImageView() {
    if (_image_view != VK_NULL_HANDLE)
        vkDestroyImageView(_logical_device.handle(), _image_view, nullptr);
}
}  // namespace Vulkan
