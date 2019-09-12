//
// Created by Dániel Molnár on 2019-08-14.
//

// ----- own header -----
#include <Renderer/Vulkan/Block.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Chunk.hpp>

namespace Vulkan::Memory {
VkDeviceMemory Block::memory() const { return _owner.memory(); }
}