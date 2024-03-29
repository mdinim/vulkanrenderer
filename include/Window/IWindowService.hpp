#pragma once
#ifndef _VULKAN_ENGINE_IWINDOWSERVICE_HPP_
#define _VULKAN_ENGINE_IWINDOWSERVICE_HPP_

// ----- std -----
#include <memory>

// ----- libraries -----

// ----- in-project dependencies -----

// ----- forward decl -----
class IWindow;
namespace Vulkan {
class Renderer;
}

class IWindowService {
   public:
    enum class RendererType {
        Vulkan
    };

    virtual std::shared_ptr<IWindow> spawn_window(unsigned width,
                                                  unsigned height,
                                                  const std::string& title) = 0;

    virtual void pre_render_hook() = 0;
    virtual void post_render_hook() = 0;

    virtual void setup(RendererType) = 0;
    // add further renderer types here

    virtual std::pair<unsigned int, const char**> get_extensions() const = 0;
};

#endif
