//
// Created by Dániel Molnár on 2019-08-14.
//

#pragma once
#ifndef VULKANENGINE_CHUNK_HPP
#define VULKANENGINE_CHUNK_HPP

// ----- std -----
#include <mutex>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

#include <Core/Graph/BinarySearchTree.hpp>
#include <Core/Utils/Size.hpp>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Memory/Block.hpp>
#include <Renderer/Vulkan/Utils.hpp>

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}

namespace Vulkan::Memory {
class Chunk {
   private:
    const LogicalDevice& _logical_device;
    using MemoryTree = Core::BinarySearchTree<Block>;

    MemoryTree _blocks;

    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkMemoryPropertyFlags _properties;

    Core::SizeLiterals::Byte _size;

    std::byte* _data;
    unsigned int _mapping_counter = 0;
    std::mutex _map_guard;

    std::optional<std::reference_wrapper<const Block>> create_suitable_node(
        Core::SizeLiterals::Byte desired_size,
        Core::SizeLiterals::Byte desired_alignment);
    void split(const Block& block);
    void try_merge(const MemoryTree::Node& block);

   public:
    Chunk(const LogicalDevice& logical_device, VkMemoryPropertyFlags properties,
          unsigned int memory_type_index, Core::SizeLiterals::Byte size);

    ~Chunk();

    [[nodiscard]] const LogicalDevice& logical_device() const {
        return _logical_device;
    }

    VkDeviceMemory memory() const { return _memory; }

    void map();
    void unmap();
    void transfer(void* data, size_t size, Core::SizeLiterals::Byte offset);

    std::optional<std::reference_wrapper<const Block>> request_memory(
        VkMemoryRequirements memory_requirements);

    void release_memory(const Block& block);
};
}  // namespace Vulkan::Memory

#endif  // VULKANENGINE_CHUNK_HPP
