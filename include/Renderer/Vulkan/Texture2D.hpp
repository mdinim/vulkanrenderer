//
// Created by Dániel Molnár on 2019-10-01.
//

#pragma once
#ifndef VULKANENGINE_TEXTURE2D_HPP
#define VULKANENGINE_TEXTURE2D_HPP

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies -----
#include <Asset/Image.hpp>
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Images.hpp>
#include <Renderer/Vulkan/Buffers.hpp>

// ----- forward-decl -----

namespace Vulkan {
class Texture2D : public Image {
   private:
    const Asset::Image& _image;

   public:
    Texture2D(LogicalDevice& logical_device, const Asset::Image& image);

    ~Texture2D() override = default;

    [[nodiscard]] VkImageViewType view_type() const override {
        return VK_IMAGE_VIEW_TYPE_2D;
    }

    void transfer(TempCommandBuffer& command_buffer, VkQueue queue);

    SubBufferDescriptor pre_stage(PolymorphBuffer<StagingBufferTag>& stage);
    void stage(TempCommandBuffer& command_buffer,
               PolymorphBuffer<StagingBufferTag>& stage,
               const SubBufferDescriptor& desc);
};
}  // namespace Vulkan

#endif  // VULKANENGINE_TEXTURE2D_HPP
