#pragma once
#ifndef _VULKAN_ENGINE_IWINDOW_HPP_
#define _VULKAN_ENGINE_IWINDOW_HPP_

#include <optional>

namespace Vulkan {
class Renderer;
}

class IWindow {
   public:
    [[nodiscard]] virtual int width() const = 0;
    [[nodiscard]] virtual int height() const = 0;

    virtual std::optional<VkSurfaceKHR> create_surface(
        Vulkan::Renderer&) const = 0;
    // add further renderer types here

    [[nodiscard]] virtual std::pair<int, int> size() const = 0;

    [[nodiscard]] virtual bool should_close() const = 0;
};

#endif
