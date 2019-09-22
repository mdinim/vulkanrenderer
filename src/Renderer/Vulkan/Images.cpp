//
// Created by Dániel Molnár on 2019-09-22.
//

// ----- own header -----
#include <Renderer/Vulkan/Images.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Memory/Block.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>

namespace Vulkan {

Image::Image(Vulkan::LogicalDevice& logical_device, VkImage image,
             unsigned int width, unsigned int height, unsigned int depth,
             unsigned int mip_levels, unsigned int array_layers,
             VkFormat format, VkImageLayout initial_layout,
             VkImageUsageFlags usage, VkSharingMode sharing_mode,
             VkMemoryPropertyFlags properties)
    : _logical_device(logical_device),
      _image(image),
      _extent{width, height, depth},
      _mip_levels(mip_levels),
      _array_layers(array_layers),
      _format(format),
      _layout(initial_layout),
      _usage(usage),
      _sharing_mode(sharing_mode),
      _properties(properties) {}

Image::Image(Vulkan::LogicalDevice& logical_device, VkImageType type,
             unsigned int width, unsigned int height, unsigned int depth,
             unsigned int mip_levels, unsigned int array_layers,
             VkFormat format, VkImageTiling tiling,
             VkImageLayout initial_layout, VkImageUsageFlags usage,
             VkSharingMode sharing_mode, VkSampleCountFlagBits samples,
             VkImageCreateFlags flags, VkMemoryPropertyFlags properties)

    : _logical_device(logical_device),
      _extent{width, height, depth},
      _mip_levels(mip_levels),
      _array_layers(array_layers),
      _format(format),
      _layout(initial_layout),
      _usage(usage),
      _sharing_mode(sharing_mode),
      _properties(properties) {
    VkImageCreateInfo create_info = {};

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = type;
    create_info.extent = {width, height, depth};
    create_info.mipLevels = mip_levels;
    create_info.format = format;
    create_info.tiling = tiling;
    create_info.initialLayout = initial_layout;
    create_info.arrayLayers = array_layers;

    create_info.usage = _usage;
    create_info.sharingMode = _sharing_mode;

    create_info.samples = samples;
    create_info.flags = flags;

    if (vkCreateImage(_logical_device.handle(), &create_info, nullptr,
                      &_image) != VK_SUCCESS) {
        throw std::runtime_error("Could not create image!");
    }

    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(_logical_device.handle(), _image, &mem_req);
    _block = &_logical_device.request_memory(mem_req, properties);

    vkBindImageMemory(_logical_device.handle(), _image, _block->memory(),
                      _block->offset().value);
}

Image::~Image() {
    if (_image != VK_NULL_HANDLE) {
        vkDestroyImage(_logical_device.handle(), _image, nullptr);
    }
    if (_block) {
        _logical_device.release_memory(*_block);
    }
}

void Image::transition_layout(VkCommandBuffer command_buffer,
                              VkImageLayout new_layout) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = _layout;
    barrier.newLayout = new_layout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = _image;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _array_layers;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;
    if (_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_HOST_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("Image layout transition not implemented!");
    };

    vkCmdPipelineBarrier(command_buffer, srcStage, dstStage, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    _layout = new_layout;
}

std::unique_ptr<ImageView> Image::create_view(
    const VkComponentMapping& mapping) const {
    return std::make_unique<ImageView>(_logical_device, *this, mapping);
}

// SwapchainImage
SwapchainImage::SwapchainImage(Swapchain& swapchain, VkImage image)
    : Image(swapchain.device(), image, swapchain.extent().width,
            swapchain.extent().height, 1, 1, 1, swapchain.format(),
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            swapchain.image_sharing_mode(),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

// Texture2D

Texture2D::Texture2D(Vulkan::LogicalDevice& logical_device, unsigned int width,
                     unsigned int height)
    : Image(logical_device, VK_IMAGE_TYPE_2D, width, height, 1, 1, 1,
            VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {}

}