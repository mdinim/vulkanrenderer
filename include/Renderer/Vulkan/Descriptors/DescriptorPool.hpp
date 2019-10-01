//
// Created by Dániel Molnár on 2019-09-21.
//

#pragma once
#ifndef VULKANENGINE_DESCRIPTORPOOL_HPP
#define VULKANENGINE_DESCRIPTORPOOL_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/LogicalDevice.hpp>

// ----- forward-decl -----
namespace Vulkan {
class DescriptorSet;
}

namespace Vulkan {
class DescriptorPool {
   private:
    const LogicalDevice& _logical_device;

    std::vector<VkDescriptorPoolSize> _pool_sizes;
    VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;

    std::vector<DescriptorSet> _descriptor_sets;

   public:
    DescriptorPool(const LogicalDevice& logical_device,
                   const std::vector<std::pair<VkDescriptorType, unsigned long>>&
                       pool_sizes,
                   unsigned long max_sets);
    ~DescriptorPool();

    std::vector<DescriptorSet> allocate_sets(
        unsigned int count, const std::vector<VkDescriptorSetLayout>& layouts);

    DescriptorSet allocate_set(const VkDescriptorSetLayout& layouts);
};
}  // namespace Vulkan

#endif  // VULKANENGINE_DESCRIPTORPOOL_HPP
