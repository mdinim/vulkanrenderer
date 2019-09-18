//
// Created by Dániel Molnár on 2019-08-14.
//

// ----- own header -----
#include <Renderer/Vulkan/Memory/Chunk.hpp>

// ----- std -----
#include <iostream>  // todo remove
#include <cstring>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>

namespace {
unsigned int get_greater_power_of_two(unsigned int of) {
    return std::pow(2, std::ceil(std::log2(of)));
}
}  // namespace

namespace Vulkan::Memory {

Chunk::Chunk(const Vulkan::LogicalDevice& logical_device,
             VkMemoryPropertyFlags properties, unsigned int memory_type_index,
             Core::SizeLiterals::Byte size)
    : _logical_device(logical_device), _properties(properties), _size(size) {
    using namespace Core::SizeLiterals;
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.memoryTypeIndex = memory_type_index;
    Core::SizeLiterals::Byte byte_size = size;
    alloc_info.allocationSize = byte_size.value;

    if (vkAllocateMemory(_logical_device.handle(), &alloc_info, nullptr,
                         &_memory) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate memory chunk!");
    }

    _blocks.insert(Block(*this, size, 0_B));
}

Chunk::~Chunk() { vkFreeMemory(_logical_device.handle(), _memory, nullptr); }

void Chunk::split(const Block& block) {
    block.set_free(false);
    _blocks.insert(Block(*this, block.size() / 2, block.offset()));
    _blocks.insert(
        Block(*this, block.size() / 2, block.offset() + (block.size() / 2)));
}

void Chunk::try_merge(const MemoryTree::Node& block) {
    const auto& get_sibling = [](const auto& node)
        -> std::optional<
            std::reference_wrapper<const decltype(_blocks)::Node>> {
        if (node.has_parent()) {
            if (node.is_left_child()) {
                return node.parent().right_child();
            } else {
                return node.parent().left_child();
            }
        }
        return std::nullopt;
    };

    if (auto sibling = get_sibling(block);
        sibling && sibling->get().value().free()) {
        auto& parent_block = block.parent();
        _blocks.remove_node(block);
        _blocks.remove_node(*sibling);

        parent_block.value().set_free(true);

        try_merge(parent_block);
    }
}

std::optional<std::reference_wrapper<const Block>> Chunk::create_suitable_node(
    Core::SizeLiterals::Byte desired_size,
    Core::SizeLiterals::Byte desired_alignment) {
    using namespace Core::SizeLiterals;
    namespace DFS = Core::DFS;
    namespace InOrder = Core::InOrder;
    auto view = DFS::View(_blocks);
    for (auto it = view.begin(); it != view.end(); ++it) {
        const auto& block = it->value();
        if (block.free() && block.size() == desired_size &&
            block.is_aligned(desired_alignment)) {
            return block;
        } else if ((!block.free() || !block.is_aligned(desired_alignment)) &&
                   block.size() <= desired_size) {
            // does not make sense to go on.
            auto in_order_succ =
                it->has_parent()
                    ? InOrder::ForwardIterator(&_blocks, it.operator->())
                    : InOrder::View(_blocks).end();
            if (in_order_succ != InOrder::View(_blocks).end()) {
                ++in_order_succ;
                ++in_order_succ;
            }

            it =
                in_order_succ != InOrder::View(_blocks).end()
                    ? DFS::ForwardIterator(&_blocks, in_order_succ.operator->())
                    : DFS::View(_blocks).end();
            --it;
        } else if (block.free()) {  // Unoccupied block, splittable
            split(block);
        }  // else we have a block that is taken but aligned and larger than
           // desired, might have a free block somewhere down the graph
    }

    return std::nullopt;
}  // namespace Vulkan::Memory

std::optional<std::reference_wrapper<const Block>> Chunk::request_memory(
    VkMemoryRequirements memory_requirements) {
    using namespace Core::SizeLiterals;
    Byte nearest = get_greater_power_of_two(memory_requirements.size);

    for (auto probable_offset = 0_B; probable_offset <= _size;
         probable_offset += memory_requirements.alignment) {
        if (auto block = _blocks.find(Block(*this, nearest, probable_offset));
            block.has_value() && block->get().value().free()) {
            block->get().value().set_free(false);
            return block.value().get().value();
        }
    }

    const auto& ret =
        create_suitable_node(nearest, memory_requirements.alignment);
    ret->get().set_free(false);

    return ret;
}

void Chunk::release_memory(const Vulkan::Memory::Block& block) {
    block.set_free(true);
    if (auto node = _blocks.find(block)) {
        try_merge(node->get());
    }
}

void Chunk::map() {
    if (!(_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        throw std::runtime_error("Trying to map non-host-visible memory!");
    }

    std::unique_lock lock(_map_guard);
    if (_mapping_counter == 0) {
        vkMapMemory(_logical_device.handle(), _memory, 0, _size.value, 0,
                    reinterpret_cast<void**>(&_data));
    }
    ++_mapping_counter;
}

void Chunk::unmap() {
    std::unique_lock lock(_map_guard);
    --_mapping_counter;
    if (_mapping_counter == 0) {
        vkUnmapMemory(_logical_device.handle(), _memory);
    }
}

void Chunk::transfer(void* data, size_t size, Core::SizeLiterals::Byte offset) {
    std::memcpy(_data + offset.value, data, size);
}

}