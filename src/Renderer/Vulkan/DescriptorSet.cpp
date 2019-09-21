//
// Created by Dániel Molnár on 2019-09-21.
//

// ----- own header -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/DescriptorSet.hpp>
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
    VkWriteDescriptorSet write_descriptor = {};

    write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor.dstSet = _descriptor_set;
    write_descriptor.dstBinding = layout.binding;
    write_descriptor.dstArrayElement = index;
    write_descriptor.descriptorType = layout.descriptorType;
    write_descriptor.descriptorCount = buffer_infos.size();

    write_descriptor.pBufferInfo = buffer_infos.data();
    write_descriptor.pImageInfo = nullptr;
    write_descriptor.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(_logical_device.handle(), buffer_infos.size(),
                           &write_descriptor, 0, nullptr);
}

}