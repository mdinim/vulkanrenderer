#pragma once
#ifndef _VULKAN_ENGINE_GLFWWINDOW_HPP_
#define _VULKAN_ENGINE_GLFWWINDOW_HPP_

#include <memory>
#include <optional>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Window/IWindow.hpp>

class VulkanRenderer;

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
        const VulkanRenderer& renderer) override;

    unsigned width() const override {
        int width, height [[maybe_unused]];
        glfwGetWindowSize(_handle.get(), &width, &height);

        return width;
    }

    unsigned height() const override {
        int width [[maybe_unused]], height;
        glfwGetWindowSize(_handle.get(), &width, &height);

        return height;
    }

    bool should_close() const override {
        return glfwWindowShouldClose(_handle.get());
    }
};

#endif
