#pragma once
#ifndef _VULKAN_ENGINE_GLFWWINDOW_HPP_
#define _VULKAN_ENGINE_GLFWWINDOW_HPP_

#include <memory>
#include <optional>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Window/IWindow.hpp>

#include <Renderer/IRenderer.hpp>

namespace Vulkan {
class Renderer;
}

class GLFWWindow : public IWindow {
   private:
    struct GLFWWindowDeleter {
       public:
        void operator()(GLFWwindow* window) { glfwDestroyWindow(window); }
    };

    std::unique_ptr<GLFWwindow, GLFWWindowDeleter> _handle;

   public:
    GLFWWindow(unsigned width, unsigned height, const std::string& title);

    virtual ~GLFWWindow() = default;

    std::optional<VkSurfaceKHR> create_surface(
        Vulkan::Renderer& renderer) const override;

    [[nodiscard]] std::pair<int, int> size() const override {
        int width, height [[maybe_unused]];
        glfwGetWindowSize(_handle.get(), &width, &height);

        return {width, height};
    }

    [[nodiscard]] int width() const override {
        auto [width, height] = size();

        return width;
    }

    [[nodiscard]] int height() const override {
        auto [width, height] = size();

        return height;
    }

    bool should_close() const override {
        return glfwWindowShouldClose(_handle.get());
    }

    static void framebuffer_resized_callback(GLFWwindow* window, int width,
                                             int height) {
        auto user_pointer =
            reinterpret_cast<IRenderer*>(glfwGetWindowUserPointer(window));
        user_pointer->resized(width, height);
    }
};

#endif
