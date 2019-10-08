//
// Created by Dániel Molnár on 2019-08-12.
//

#pragma once
#ifndef VULKANENGINE_COMMANDPOOL_HPP
#define VULKANENGINE_COMMANDPOOL_HPP

// ----- std -----
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>
#include <configuration.hpp>
#include <iostream>  // todo remove

// ----- in-project dependencies -----

// ----- forward-decl -----
namespace Vulkan {
class Swapchain;
class TempCommandBuffer;
class LogicalDevice;
}  // namespace Vulkan

namespace Vulkan {
class CommandPool {
   private:
    const Swapchain& _swapchain;

    VkCommandPool _command_pool;

    std::vector<VkCommandBuffer> _command_buffers;

    mutable unsigned int _current_offset = 0;
    unsigned int _requested_size = 0;

   public:
    CommandPool(const Swapchain& swapchain);
    ~CommandPool();

    [[nodiscard]] const VkCommandPool& handle() const { return _command_pool; }

    void allocate_buffers(unsigned int count);
    void free_buffers();

    [[nodiscard]] TempCommandBuffer allocate_temp_buffer() const;

    [[nodiscard]] const VkCommandBuffer& buffer(unsigned int i,
                                                unsigned int batch = 0) const;

    [[nodiscard]] size_t size() const { return _command_buffers.size(); }
    void shift() const;
};

class TempCommandBuffer {
   private:
    friend class CommandPool;
    const CommandPool& _command_pool;
    const LogicalDevice& _logical_device;

    VkCommandBuffer _command_buffer = VK_NULL_HANDLE;

    TempCommandBuffer(const CommandPool& command_pool,
                      const LogicalDevice& logical_device);

   public:
    [[nodiscard]] const VkCommandBuffer& handle() const {
        return _command_buffer;
    }

    void flush(VkQueue queue);

    ~TempCommandBuffer();
};
}  // namespace Vulkan

#endif  // VULKANENGINE_COMMANDPOOL_HPP
