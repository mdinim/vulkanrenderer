//
// Created by Dániel Molnár on 2019-08-14.
//

#pragma once
#ifndef VULKANENGINE_BLOCK_HPP
#define VULKANENGINE_BLOCK_HPP

// ----- std -----

// ----- libraries -----
#include <Core/Utils/Size.hpp>

#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----

// ----- forward-decl -----

namespace Vulkan::Memory {
class Chunk;
}

namespace Vulkan::Memory {
// TODO Make move-only
class Block {
   private:
    const Chunk& _owner;
    Core::SizeLiterals::Byte _size;
    Core::SizeLiterals::Byte _offset;
    mutable bool _free = true;

   public:
    Block(const Chunk& owner, Core::SizeLiterals::Byte size,
          Core::SizeLiterals::Byte offset, bool free = true)
        : _owner(owner), _size(size), _offset(offset), _free(free) {}

    bool is_aligned(Core::SizeLiterals::Byte alignment) const {
        return _offset.value % alignment.value == 0;
    }

    bool free() const { return _free; }
    void set_free(bool value) const { _free = value; }
    Core::SizeLiterals::Byte size() const { return _size; }
    Core::SizeLiterals::Byte offset() const { return _offset; }
    VkDeviceMemory memory() const;

    bool operator==(const Block& other) const {
        return other._offset == _offset && other._size == _size;
    }

    bool operator!=(const Block& other) const { return !(other == *this); }

    bool operator>(const Block& other) const {
        return _size < other._size &&
               _offset >= other._offset + (other._size.value / 2);
    }
};
}  // namespace Vulkan::Memory

#endif  // VULKANENGINE_BLOCK_HPP
