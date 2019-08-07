#pragma once
#ifndef _VULKAN_ENGINE_GLFWWINDOWSERVICE_HPP_
#define _VULKAN_ENGINE_GLFWWINDOWSERVICE_HPP_

#include <memory>
#include <vector>

#include <Window/IWindow.hpp>
#include <Window/IWindowService.hpp>

namespace Vulkan {
class Renderer;
}

class GLFWWindowService : public IWindowService {
   private:
    std::vector<std::shared_ptr<IWindow>> _windows;

   public:
    GLFWWindowService();
    virtual ~GLFWWindowService();

    std::shared_ptr<IWindow> spawn_window(unsigned width, unsigned height,
                                          const std::string& title) override;

    void pre_render_hook() override;
    void post_render_hook() override;

    void setup(const Vulkan::Renderer&) override;
    // Add further renderere types here

    std::pair<unsigned int, const char**> get_extensions() const override;
};

#endif
