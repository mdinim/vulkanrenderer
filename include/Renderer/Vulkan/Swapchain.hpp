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
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/Images.hpp>
#include <Renderer/Vulkan/Pipelines/IPipeline.hpp>
#include <Renderer/Vulkan/RenderPass.hpp>

// ----- forward-decl -----

namespace Vulkan {
class PhysicalDevice;
class Surface;
class LogicalDevice;
class RenderPass;
class IPipeline;
class SwapchainImage;
}  // namespace Vulkan

namespace Vulkan {
class Swapchain {
   private:
    const Surface& _surface;
    const PhysicalDevice& _physical_device;
    LogicalDevice& _logical_device;

    VkSwapchainKHR _swapchain;
    VkExtent2D _extent;
    VkFormat _format;

    VkSharingMode _image_sharing_mode;

    std::vector<SwapchainImage> _images;
    std::vector<std::unique_ptr<ImageView>> _image_views;
    std::vector<Framebuffer> _framebuffers;

    std::unique_ptr<RenderPass> _render_pass;
    std::vector<std::unique_ptr<IPipeline>> _pipelines;

    CommandPool _command_pool;

    void create();
    void teardown();

   public:
    Swapchain(const Surface& surface, const PhysicalDevice& physical_device,
              LogicalDevice& logical_device);
    ~Swapchain();

    void recreate();

    [[nodiscard]] const Surface& surface() const { return _surface; }
    [[nodiscard]] const PhysicalDevice& physical_device() const {
        return _physical_device;
    }
    [[nodiscard]] LogicalDevice& device() const { return _logical_device; }

    [[nodiscard]] const CommandPool& command_pool() const {
        return _command_pool;
    }
    [[nodiscard]] const VkSwapchainKHR& handle() const { return _swapchain; }
    [[nodiscard]] const VkExtent2D& extent() const { return _extent; }
    [[nodiscard]] const VkFormat& format() const { return _format; }
    [[nodiscard]] const std::vector<SwapchainImage>& images() const {
        return _images;
    }
    [[nodiscard]] const VkSharingMode& image_sharing_mode() const {
        return _image_sharing_mode;
    }

    [[nodiscard]] const std::vector<Framebuffer>& framebuffers() const {
        return _framebuffers;
    }

    [[nodiscard]] const RenderPass& render_pass() const {
        return *_render_pass;
    }

    template <class PipelineType, class... Args>
    IPipeline& attach_pipeline(Args... args) {
        static_assert(std::is_base_of_v<IPipeline, PipelineType>,
                      "Not a pipeline type!");
        auto& ref = _pipelines.emplace_back(
            std::make_unique<PipelineType>(*this, args...));
        return *ref;
    }

    VkResult acquireNextImage(unsigned int& index, VkSemaphore signal);

    VkResult present(unsigned int index, VkSemaphore wait);
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SWAPCHAIN_HPP
