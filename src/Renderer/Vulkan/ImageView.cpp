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
ImageView::ImageView(const Vulkan::LogicalDevice& logical_device, VkImage image,
                     const Vulkan::Swapchain& swapchain)
    : _logical_device(logical_device), _swapchain(swapchain) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = swapchain.format();

    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;

    if (vkCreateImageView(_logical_device.handle(), &create_info, nullptr,
                          &_image_view) != VK_SUCCESS) {
        throw std::runtime_error("Could not create image view");
    }
}

ImageView::ImageView(ImageView&& other)
    : _logical_device(other._logical_device), _swapchain(other._swapchain) {
    _image_view = other._image_view;
    other._image_view = VK_NULL_HANDLE;
}

ImageView::~ImageView() {
    if (_image_view != VK_NULL_HANDLE)
        vkDestroyImageView(_logical_device.handle(), _image_view, nullptr);
}
}  // namespace Vulkan
