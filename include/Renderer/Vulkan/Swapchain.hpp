//
// Created by Dániel Molnár on 2019-08-09.
//

#pragma once
#ifndef VULKANENGINE_SWAPCHAIN_HPP
#define VULKANENGINE_SWAPCHAIN_HPP

// ----- std -----
#include <memory>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan_core.h>

// ----- in-project dependencies -----
#include <Renderer/Vulkan/CommandPool.hpp>
#include <Renderer/Vulkan/Framebuffer.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/RenderPass.hpp>

// ----- forward-decl -----

namespace Vulkan {
class PhysicalDevice;
class Surface;
class LogicalDevice;
class RenderPass;
}  // namespace Vulkan

namespace Vulkan {
class Swapchain {
   private:
    const Surface& _surface;
    const PhysicalDevice& _physical_device;
    const LogicalDevice& _logical_device;

    VkSwapchainKHR _swapchain;
    VkExtent2D _extent;
    VkFormat _format;

    std::vector<VkImage> _images;
    std::vector<ImageView> _image_views;
    std::vector<Framebuffer> _framebuffers;

    std::unique_ptr<RenderPass> _render_pass;
    std::unique_ptr<GraphicsPipeline> _graphics_pipeline;

    CommandPool _command_pool;

    void create();
    void teardown();

   public:
    Swapchain(const Surface& surface, const PhysicalDevice& physical_device,
              const LogicalDevice& logical_device);
    ~Swapchain();

    void recreate();

    [[nodiscard]] const Surface& surface() const { return _surface; }
    [[nodiscard]] const PhysicalDevice& physical_device() const {
        return _physical_device;
    }
    [[nodiscard]] const LogicalDevice& device() const {
        return _logical_device;
    }
    [[nodiscard]] const CommandPool& command_pool() const {
        return _command_pool;
    }
    [[nodiscard]] const VkSwapchainKHR& handle() const { return _swapchain; }
    [[nodiscard]] const VkExtent2D& extent() const { return _extent; }
    [[nodiscard]] const VkFormat& format() const { return _format; }
    [[nodiscard]] const std::vector<VkImage>& images() const { return _images; }
    [[nodiscard]] const std::vector<ImageView>& image_views() const {
        return _image_views;
    }
    [[nodiscard]] const std::vector<Framebuffer>& framebuffers() const {
        return _framebuffers;
    }
    const std::vector<VkCommandBuffer>& buffers() {
        return _command_pool.buffers();
    }
    [[nodiscard]] const RenderPass& render_pass() const {
        return *_render_pass;
    }
    [[nodiscard]] const GraphicsPipeline& graphics_pipeline() const {
        return *_graphics_pipeline;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SWAPCHAIN_HPP
