//
// Created by Dániel Molnár on 2019-09-21.
//

#pragma once
#ifndef VULKANENGINE_DESCRIPTORSET_HPP
#define VULKANENGINE_DESCRIPTORSET_HPP

// ----- std -----
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Buffers.hpp>

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}  // namespace Vulkan

namespace Vulkan {

class DescriptorSet {
   private:
    const LogicalDevice& _logical_device;
    VkDescriptorPool _owner;
    VkDescriptorSet _descriptor_set;

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               std::vector<VkDescriptorBufferInfo> buffer_infos);
   public:
    DescriptorSet(const LogicalDevice& logical_device, VkDescriptorPool pool,
                  VkDescriptorSet set);

    [[nodiscard]] const VkDescriptorSet& handle() const {
        return _descriptor_set;
    }

    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const Buffer& buffer);

    void write(
        VkDescriptorSetLayoutBinding layout, unsigned int index,
        const std::vector<std::reference_wrapper<const Buffer>>& buffers);

    template <class... SubBuffers>
    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const PolymorphBuffer<SubBuffers...>& buffer,
               SubBufferDescriptor dest_buffer_descriptor) {
        write(layout, index, buffer, {dest_buffer_descriptor});
    }

    template <class... SubBuffers>
    void write(VkDescriptorSetLayoutBinding layout, unsigned int index,
               const PolymorphBuffer<SubBuffers...>& buffer,
               std::vector<SubBufferDescriptor> dest_buffer_descriptors) {
        std::vector<VkDescriptorBufferInfo> buffer_infos;
        buffer_infos.reserve(dest_buffer_descriptors.size());
        for (const auto& descriptor : dest_buffer_descriptors) {
            VkDescriptorBufferInfo buffer_info = {};
            buffer_info.buffer = buffer.handle();
            buffer_info.offset = descriptor.offset;
            buffer_info.range = descriptor.size;

            buffer_infos.push_back(buffer_info);
        }

        write(layout, index, buffer_infos);
    }
};

}  // namespace Vulkan

#endif  // VULKANENGINE_DESCRIPTORSET_HPP
