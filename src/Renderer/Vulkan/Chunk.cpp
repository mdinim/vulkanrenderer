//
// Created by Dániel Molnár on 2019-08-14.
//

// ----- own header -----
#include <Renderer/Vulkan/Chunk.hpp>

// ----- std -----
#include <iostream>  // todo remove

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
             unsigned int memory_type_index, Core::SizeLiterals::Byte size)
    : _logical_device(logical_device), _size(size) {
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

std::optional<std::reference_wrapper<const Block>> Chunk::request_memory(
    VkMemoryRequirements memory_requirements) {
    using namespace Core::SizeLiterals;
    Byte nearest = get_greater_power_of_two(memory_requirements.size);

    for (auto probable_offset = 0_B; probable_offset <= _size;
         probable_offset += memory_requirements.alignment) {
        if (auto block = _blocks.find(Block(*this, nearest, probable_offset));
            block.has_value() && block->get().value().free()) {
            return block.value().get().value();
        }
    }

    bool has_split = false;
    int i = 0;
    do {
        has_split = false;
        for (auto& node : Core::InOrder::View(_blocks)) {
            i++;
            if (node.value().free() && node.value().size() > nearest) {
                split(node.value());
                has_split = true;
                break;
            } else if (node.value().free() && node.value().size() == nearest) {
                return node.value();
            }
        }
    } while (has_split);

    for (auto& node : Core::InOrder::View(_blocks)) {
        std::cout << node.value().offset() << " " << node.value().size()
                  << std::endl;
    }

    return std::nullopt;
}

}