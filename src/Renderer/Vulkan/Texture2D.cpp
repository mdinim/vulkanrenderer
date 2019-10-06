//
// Created by Dániel Molnár on 2019-10-01.
//

// ----- own header -----
#include <Renderer/Vulkan/Texture2D.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Data/Representation.hpp>
#include <Renderer/Vulkan/Descriptors/DescriptorPool.hpp>
#include <Renderer/Vulkan/Descriptors/DescriptorSetLayout.hpp>

namespace Vulkan {
Texture2D::Texture2D(LogicalDevice& logical_device, const Asset::Image& image)
    : Image(logical_device, VK_IMAGE_TYPE_2D, image.width(), image.height(), 1,
            1, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SHARING_MODE_EXCLUSIVE, VK_SAMPLE_COUNT_1_BIT, 0,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
      _image_asset(image) {
    _view = create_view(VK_IMAGE_ASPECT_COLOR_BIT);
}

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
    return stage.commit_sub_buffer<StagingBufferTag>(_image_asset.size());
}

void Texture2D::stage(TempCommandBuffer& command_buffer,
                      PolymorphBuffer<StagingBufferTag>& stage,
                      const SubBufferDescriptor& desc) {
    stage.transfer((void*)_image_asset.data(), desc);
    stage.copy_to(command_buffer, desc, *this);
}

void Texture2D::attach_desc_pool(DescriptorPool* pool,
                                 DescriptorSetLayout* layout,
                                 VkSampler sampler) {
    _descriptor_pool = pool;
    _descriptor_set_layout = layout;
    _descriptor_set =
        _descriptor_pool->allocate_set(_descriptor_set_layout->handle());
    _descriptor_set->write(Texture_sampler_descriptor(), 0, *this, *_view,
                           sampler);
    _descriptor_set->update();
}

VkDescriptorSet Texture2D::desc_handle() const {
    return _descriptor_set->handle();
}

}  // namespace Vulkan
