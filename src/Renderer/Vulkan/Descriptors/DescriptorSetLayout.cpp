//
// Created by Dániel Molnár on 2019-10-01.
//

// ----- own header -----
#include "Renderer/Vulkan/Descriptors/DescriptorSetLayout.hpp"

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>

namespace Vulkan {
DescriptorSetLayout::DescriptorSetLayout(
    LogicalDevice& logical_device,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    : _logical_device(logical_device) {
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
    descriptor_set_layout_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = bindings.size();
    descriptor_set_layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_logical_device.handle(),
                                    &descriptor_set_layout_info, nullptr,
                                    &_layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create descriptor sett layout");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(_logical_device.handle(), _layout, nullptr);
}
}