//
// Created by Dániel Molnár on 2019-10-01.
//

#pragma once
#ifndef VULKANENGINE_DESCRIPTORSETLAYOUT_HPP
#define VULKANENGINE_DESCRIPTORSETLAYOUT_HPP

// ----- std -----
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}

namespace Vulkan {
class DescriptorSetLayout {
   private:
    LogicalDevice& _logical_device;

    VkDescriptorSetLayout _layout = VK_NULL_HANDLE;

   public:
    DescriptorSetLayout(
        LogicalDevice& logical_device,
        const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    ~DescriptorSetLayout();

    [[nodiscard]] VkDescriptorSetLayout handle() const { return _layout; }
};

}  // namespace Vulkan

#endif  // VULKANENGINE_DESCRIPTORSETLAYOUT_HPP
