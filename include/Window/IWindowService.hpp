#pragma once
#ifndef _VULKAN_ENGINE_IWINDOWSERVICE_HPP_
#define _VULKAN_ENGINE_IWINDOWSERVICE_HPP_

#include <memory>

class IWindow;
class VulkanRenderer;

class IWindowService {
   public:
    virtual std::shared_ptr<IWindow> spawn_window(unsigned width,
                                                  unsigned height,
                                                  const std::string& title) = 0;

    virtual void pre_render_hook() = 0;
    virtual void post_render_hook() = 0;

    virtual void setup(const VulkanRenderer&) = 0;
    // add further renderer types here

    virtual std::pair<unsigned int, const char**> get_extensions() const = 0;
};

#endif
