//
// Created by Dániel Molnár on 2019-09-21.
//

// ----- own header -----
#include <Renderer/Vulkan/Descriptors/DescriptorPool.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Descriptors/DescriptorSet.hpp>

namespace Vulkan {

DescriptorPool::DescriptorPool(
    const Vulkan::LogicalDevice& logical_device,
    const std::vector<std::pair<VkDescriptorType, unsigned long>>& pool_sizes,
    unsigned long max_sets)
    : _logical_device(logical_device) {
    for (const auto& [type, count] : pool_sizes) {
        VkDescriptorPoolSize pool_size;
        pool_size.type = type;
        pool_size.descriptorCount = count;

        _pool_sizes.push_back(pool_size);
    }

    VkDescriptorPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    create_info.poolSizeCount = _pool_sizes.size();
    create_info.pPoolSizes = _pool_sizes.data();
    create_info.maxSets = max_sets;

    if (vkCreateDescriptorPool(_logical_device.handle(), &create_info, nullptr,
                               &_descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create descriptor pool");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(_logical_device.handle(), _descriptor_pool,
                            nullptr);
}

std::vector<DescriptorSet*> DescriptorPool::allocate_sets(
    unsigned int count, const std::vector<VkDescriptorSetLayout>& layouts) {
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    allocate_info.descriptorPool = _descriptor_pool;
    allocate_info.descriptorSetCount = count;
    allocate_info.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> desc_sets;
    desc_sets.resize(count);
    if (vkAllocateDescriptorSets(_logical_device.handle(), &allocate_info,
                                 desc_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate descriptor sets!");
    }

    std::vector<DescriptorSet*> result;
    result.reserve(desc_sets.size());
    for (const auto& desc_set : desc_sets) {
        auto& desc = _descriptor_sets.emplace_back(_logical_device,
                                                   _descriptor_pool, desc_set);
        result.emplace_back(&desc);
    }

    return result;
}

DescriptorSet* DescriptorPool::allocate_set(
    const VkDescriptorSetLayout& layout) {
    auto result = allocate_sets(1, {layout});

    return result[0];
}

}