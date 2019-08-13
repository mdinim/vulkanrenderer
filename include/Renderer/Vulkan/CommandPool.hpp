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
}

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

    VkCommandBuffer buffer(unsigned int i) { return _command_buffers.at(i); }
    [[nodiscard]] const std::vector<VkCommandBuffer>& buffers() const {
        return _command_buffers;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_COMMANDPOOL_HPP
