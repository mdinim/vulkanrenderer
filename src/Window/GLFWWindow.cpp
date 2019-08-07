#include <Renderer/Vulkan/Renderer.hpp>
#include <Window/GLFWWindow.hpp>

GLFWWindow::GLFWWindow(unsigned width, unsigned height,
                       const std::string& title) {
    _handle = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(
        glfwCreateWindow(width, height, title.data(), nullptr, nullptr));

    glfwSetFramebufferSizeCallback(_handle.get(),
                                   GLFWWindow::framebuffer_resized_callback);
}

std::optional<VkSurfaceKHR> GLFWWindow::create_surface(
    Vulkan::Renderer& renderer) const {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(renderer.get_instance(), _handle.get(), nullptr,
                                &surface) != VK_SUCCESS) {
        return {};
    }

    glfwSetWindowUserPointer(_handle.get(), &renderer);
    return surface;
}
