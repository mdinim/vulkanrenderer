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

namespace Vulkan {
CommandPool::CommandPool(const Vulkan::Swapchain& swapchain)
    : _swapchain(swapchain) {
    auto queue_family_indices = Vulkan::Utils::FindQueueFamilies(
        _swapchain.physical_device(), _swapchain.surface());

    VkCommandPoolCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    create_info.flags = 0;
    create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

    if (vkCreateCommandPool(swapchain.device().handle(), &create_info, nullptr,
                            &_command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command pool");
    }
}
CommandPool::~CommandPool() {
    vkDestroyCommandPool(_swapchain.device().handle(), _command_pool, nullptr);
}
}  // namespace Vulkan
