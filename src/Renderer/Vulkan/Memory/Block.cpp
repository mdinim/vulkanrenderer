//
// Created by Dániel Molnár on 2019-08-14.
//

// ----- own header -----
#include <Renderer/Vulkan/Memory/Block.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Memory/Chunk.hpp>

namespace Vulkan::Memory {
VkDeviceMemory Block::memory() const { return _owner.memory(); }

void Block::transfer(void* data, size_t size, size_t target_offset) const {
    _owner.map();
    _owner.transfer(data, size, _offset + target_offset);
    _owner.unmap();
}

}  // namespace Vulkan::Memory
