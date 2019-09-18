//
// Created by Dániel Molnár on 2019-08-14.
//

#pragma once
#ifndef VULKANENGINE_ALLOCATOR_HPP
#define VULKANENGINE_ALLOCATOR_HPP

// ----- std -----
#include <map>
#include <vector>

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Memory/Block.hpp>
#include <Renderer/Vulkan/Memory/Chunk.hpp>

// ----- forward-decl -----
namespace Vulkan {
class PhysicalDevice;
class LogicalDevice;

namespace Memory {
class Block;
}
}

namespace Vulkan::Memory {
class Allocator {
   private:
    const PhysicalDevice& _physical_device;
    const LogicalDevice& _logical_device;

    std::multimap<unsigned int, Chunk> _chunks;

   public:
    Allocator(const PhysicalDevice& physical_device,
              const LogicalDevice& logical_device);

    void deallocate();
    const Block& request_memory(VkMemoryRequirements memory_requirements,
                                VkMemoryPropertyFlags properties);
    void release_memory(const Block& block);
};
}  // namespace Vulkan::Memory

#endif  // VULKANENGINE_ALLOCATOR_HPP
