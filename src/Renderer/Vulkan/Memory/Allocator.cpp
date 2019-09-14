//
// Created by Dániel Molnár on 2019-08-14.
//

// ----- own header -----
#include <Renderer/Vulkan/Memory/Allocator.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Memory/Chunk.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>

namespace Vulkan::Memory {

Allocator::Allocator(const PhysicalDevice& physical_device,
                     const LogicalDevice& logical_device)
    : _physical_device(physical_device), _logical_device(logical_device) {}

void Allocator::deallocate() { _chunks.clear(); }

const Block& Allocator::request_memory(VkMemoryRequirements memory_requirements,
                                       VkMemoryPropertyFlags properties) {
    using namespace Core::SizeLiterals;
    auto memory_type_index = Utils::FindMemoryType(
        _physical_device, memory_requirements.memoryTypeBits, properties);
    auto [begin, end] = _chunks.equal_range(memory_type_index);
    for (auto it = begin; it != end; ++it) {
        if (auto block = it->second.request_memory(memory_requirements))
            return block->get();
    }
    auto new_it = _chunks.emplace(
        std::piecewise_construct, std::forward_as_tuple(memory_type_index),
        std::forward_as_tuple(_logical_device, memory_type_index, 256_MB));

    return new_it->second.request_memory(memory_requirements)->get();
}
}  // namespace Vulkan::Memory
