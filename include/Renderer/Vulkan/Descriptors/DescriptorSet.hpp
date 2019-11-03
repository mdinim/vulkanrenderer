//
// Created by Dániel Molnár on 2019-09-21.
//

#pragma once
#ifndef VULKANENGINE_DESCRIPTORSET_HPP
#define VULKANENGINE_DESCRIPTORSET_HPP

// ----- std -----
#include <optional>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Buffers.hpp>
#include <Renderer/Vulkan/Images.hpp>

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}  // namespace Vulkan

namespace Vulkan {

class DescriptorSet {
   private:
    struct ScheduledDescriptorWrite {
        ScheduledDescriptorWrite(
            unsigned int index, VkDescriptorSetLayoutBinding layout,
            std::vector<VkDescriptorBufferInfo>&& buffer_infos)
            : target_index(index), target_layout(layout), infos(buffer_infos) {}

        ScheduledDescriptorWrite(
            unsigned int index, VkDescriptorSetLayoutBinding layout,
            std::vector<VkDescriptorImageInfo>&& image_infos)
            : target_index(index), target_layout(layout), infos(image_infos) {}

        unsigned int target_index;
        VkDescriptorSetLayoutBinding target_layout;

        std::variant<std::vector<VkDescriptorBufferInfo>,
                     std::vector<VkDescriptorImageInfo>>
            infos;
    };

    const LogicalDevice& _logical_device;
    VkDescriptorPool _owner [[maybe_unused]];
    VkDescriptorSet _descriptor_set;

    std::vector<ScheduledDescriptorWrite> _scheduled_writes;

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               std::vector<VkDescriptorBufferInfo> buffer_infos);

   public:
    DescriptorSet(const LogicalDevice& logical_device, VkDescriptorPool pool,
                  VkDescriptorSet set);

    DescriptorSet(const DescriptorSet& other) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    DescriptorSet(DescriptorSet&& other) = delete;
    DescriptorSet& operator=(DescriptorSet&&) = delete;

    ~DescriptorSet() = default;

    [[nodiscard]] const VkDescriptorSet& handle() const {
        return _descriptor_set;
    }

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const Buffer& buffer);

    void write(
        VkDescriptorSetLayoutBinding layout, unsigned int index,
        const std::vector<std::reference_wrapper<const Buffer>>& buffers);

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const Buffer& buffer,
               SubBufferDescriptor buffer_descriptor) {
        write(layout, index, buffer, {{buffer_descriptor}});
    }

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const Buffer& buffer,
               const std::vector<SubBufferDescriptor>& buffer_descriptors) {
        std::vector<VkDescriptorBufferInfo> buffer_infos;
        buffer_infos.reserve(buffer_descriptors.size());
        for (const auto& descriptor : buffer_descriptors) {
            VkDescriptorBufferInfo buffer_info = {};
            buffer_info.buffer = buffer.handle();
            buffer_info.offset = descriptor.offset;
            buffer_info.range = descriptor.size;

            buffer_infos.push_back(buffer_info);
        }

        write(layout, index, buffer_infos);
    }

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const Image& image, const ImageView& view, VkSampler sampler);

    void update();
};

}  // namespace Vulkan

#endif  // VULKANENGINE_DESCRIPTORSET_HPP
