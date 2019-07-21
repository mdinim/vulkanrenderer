#pragma once
#ifndef _VULKAN_ENGINE_IWINDOW_HPP_
#define _VULKAN_ENGINE_IWINDOW_HPP_

#include <optional>

class VulkanRenderer;

class IWindow {
public:
    virtual unsigned width() const = 0;
    virtual unsigned height() const = 0;

    virtual std::optional<VkSurfaceKHR> create_surface(const VulkanRenderer&) = 0;
    // add further renderer types here

    virtual bool should_close() const = 0;
};

#endif
