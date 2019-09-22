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

   public:
    CommandPool(const Swapchain& swapchain);
    ~CommandPool();

    [[nodiscard]] const VkCommandPool& handle() const { return _command_pool; }

    void allocate_buffers();
    void free_buffers();

    [[nodiscard]] TempCommandBuffer allocate_temp_buffer() const;

    VkCommandBuffer buffer(unsigned int i) { return _command_buffers.at(i); }
    [[nodiscard]] const std::vector<VkCommandBuffer>& buffers() const {
        return _command_buffers;
    }
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
