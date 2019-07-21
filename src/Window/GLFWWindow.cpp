#include <Window/GLFWWindow.hpp>
#include <Renderer/VulkanRenderer.hpp>

GLFWWindow::GLFWWindow(unsigned width, unsigned height,
                       const std::string& title) {
    _handle = std::unique_ptr<GLFWwindow, GLFWWindowDeleter>(
        glfwCreateWindow(width, height, title.data(), nullptr, nullptr));
}

std::optional<VkSurfaceKHR> GLFWWindow::create_surface(
    const VulkanRenderer& renderer) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(renderer.get_instance(), _handle.get(), nullptr,
                                &surface) != VK_SUCCESS) {
        return {};
    }

    return surface;
}