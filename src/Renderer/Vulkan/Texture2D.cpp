//
// Created by Dániel Molnár on 2019-10-01.
//

// ----- own header -----
#include <Renderer/Vulkan/Texture2D.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies

namespace Vulkan {
Texture2D::Texture2D(LogicalDevice& logical_device,
                     const Asset::Image& image)
    : Image(logical_device, VK_IMAGE_TYPE_2D, image.width(), image.height(), 1,
            1, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      _image(image) {}

void Texture2D::transfer(Vulkan::TempCommandBuffer& command_buffer,
                         VkQueue queue) {
    auto stage_buf =
        std::make_unique<PolymorphBuffer<StagingBufferTag>>(_logical_device);
    auto desc = pre_stage(*stage_buf);
    stage_buf->allocate();
    stage(command_buffer, *stage_buf, desc);

    command_buffer.flush(queue);
}

SubBufferDescriptor Texture2D::pre_stage(
    Vulkan::PolymorphBuffer<Vulkan::StagingBufferTag>& stage) {
    return stage.commit_sub_buffer<StagingBufferTag>(_image.size());
}

void Texture2D::stage(TempCommandBuffer& command_buffer,
                      PolymorphBuffer<StagingBufferTag>& stage,
                      const SubBufferDescriptor& desc) {
    stage.transfer((void*)_image.data(), desc);
    stage.copy_to(command_buffer, desc, *this);
}

}  // namespace Vulkan
