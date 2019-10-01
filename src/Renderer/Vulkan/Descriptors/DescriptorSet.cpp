//
// Created by Dániel Molnár on 2019-09-21.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- std -----

// ----- libraries -----
#include <Core/Utils/Utils.hpp>

// ----- in-project dependencies
#include <Renderer/Vulkan/Descriptors/DescriptorSet.hpp>
#include <Renderer/Vulkan/Images.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
namespace Vulkan {

DescriptorSet::DescriptorSet(const Vulkan::LogicalDevice& logical_device,
                             VkDescriptorPool pool, VkDescriptorSet set)
    : _logical_device(logical_device), _owner(pool), _descriptor_set(set) {}

void DescriptorSet::write(VkDescriptorSetLayoutBinding layout,
                          unsigned int index, const Buffer& buffer) {
    write(layout, index, std::vector{std::cref(buffer)});
}

void DescriptorSet::write(
    VkDescriptorSetLayoutBinding layout, unsigned int index,
    const std::vector<std::reference_wrapper<const Buffer>>& buffers) {
    std::vector<VkDescriptorBufferInfo> buffer_infos;
    buffer_infos.reserve(buffers.size());
    for (const auto& buffer : buffers) {
        VkDescriptorBufferInfo buffer_info = {};
        buffer_info.buffer = buffer.get().handle();
        buffer_info.offset = 0;
        buffer_info.range = buffer.get().size();

        buffer_infos.push_back(buffer_info);
    }

    write(layout, index, std::move(buffer_infos));
}

void DescriptorSet::write(VkDescriptorSetLayoutBinding layout,
                          unsigned int index,
                          std::vector<VkDescriptorBufferInfo> buffer_infos) {
    _scheduled_writes.emplace_back(index, layout, std::move(buffer_infos));
}

void DescriptorSet::write(VkDescriptorSetLayoutBinding layout,
                          unsigned int index, const Image& image,
                          const ImageView& view, VkSampler sampler) {
    VkDescriptorImageInfo image_info = {};
    image_info.imageLayout = image.layout();
    image_info.imageView = view.handle();
    image_info.sampler = sampler;

    _scheduled_writes.emplace_back(index, layout, std::vector{image_info});
}

void DescriptorSet::update() {
    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(_scheduled_writes.size());

    VkWriteDescriptorSet write_descriptor = {};

    write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor.dstSet = _descriptor_set;
    for (auto& scheduled_write : _scheduled_writes) {
        write_descriptor.dstBinding = scheduled_write.target_layout.binding;
        write_descriptor.dstArrayElement = scheduled_write.target_index;
        write_descriptor.descriptorType =
            scheduled_write.target_layout.descriptorType;

        Core::visit_variant(
            scheduled_write.infos,
            [&write_descriptor](
                const std::vector<VkDescriptorBufferInfo>& buffer_infos) {
                write_descriptor.descriptorCount = buffer_infos.size();
                write_descriptor.pBufferInfo = buffer_infos.data();
            },
            [&write_descriptor](
                const std::vector<VkDescriptorImageInfo>& image_infos) {
                write_descriptor.descriptorCount = image_infos.size();
                write_descriptor.pImageInfo = image_infos.data();
            });

        writes.push_back(write_descriptor);
    }

    vkUpdateDescriptorSets(_logical_device.handle(), writes.size(),
                           writes.data(), 0, nullptr);
    _scheduled_writes.clear();
}

}