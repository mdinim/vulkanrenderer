//
// Created by Dániel Molnár on 2019-08-12.
//

// ----- own header -----
#include <Renderer/Vulkan/CommandPool.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <configuration.hpp>

namespace Vulkan {
CommandPool::CommandPool(const Vulkan::Swapchain& swapchain)
    : _swapchain(swapchain) {
    auto queue_family_indices = Vulkan::Utils::FindQueueFamilies(
        _swapchain.physical_device(), _swapchain.surface());

    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

    if (vkCreateCommandPool(swapchain.device().handle(), &create_info, nullptr,
                            &_command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command pool");
    }
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(_swapchain.device().handle(), _command_pool, nullptr);
}

void CommandPool::allocate_buffers(unsigned int count) {
    _requested_size = count;
    _command_buffers.resize(count * Configuration::CommandPoolFactor);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = _command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = _command_buffers.size();

    if (vkAllocateCommandBuffers(_swapchain.device().handle(), &alloc_info,
                                 _command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate command buffers");
    }
}

void CommandPool::free_buffers() {
    vkFreeCommandBuffers(_swapchain.device().handle(), _command_pool,
                         _command_buffers.size(), _command_buffers.data());
}

TempCommandBuffer::TempCommandBuffer(
    const Vulkan::CommandPool& command_pool,
    const Vulkan::LogicalDevice& logical_device)
    : _command_pool(command_pool), _logical_device(logical_device) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = _command_pool.handle();
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(_logical_device.handle(), &alloc_info,
                                 &_command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate temporary command buffer");
    }

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_command_buffer, &begin_info);
}

void CommandPool::shift() const {
    _current_offset = (_current_offset + 1) % Configuration::CommandPoolFactor;
}

const VkCommandBuffer& CommandPool::buffer(unsigned int i,
                                           unsigned int batch) const {
    std::cout << "Batch: " << batch << " Offset: " << _current_offset << std::endl;
    return _command_buffers.at(
        ((_current_offset + batch) % Configuration::CommandPoolFactor) *
            _requested_size +
        i);
}

TempCommandBuffer CommandPool::allocate_temp_buffer() const {
    return TempCommandBuffer{*this, _swapchain.device()};
}

TempCommandBuffer::~TempCommandBuffer() {
    vkFreeCommandBuffers(_logical_device.handle(), _command_pool.handle(), 1,
                         &_command_buffer);
}

void TempCommandBuffer::flush(VkQueue queue) {
    vkEndCommandBuffer(_command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &_command_buffer;

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
}

}  // namespace Vulkan
