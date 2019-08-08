//
// Created by Dániel Molnár on 2019-07-20.
//

// ----- own header -----
#include <Window/GLFWWindowService.hpp>

// ----- std -----
#include <iostream>

// ----- libraries -----

// ----- in-project dependencies
#include <Window/GLFWWindow.hpp>

GLFWWindowService::GLFWWindowService() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("GLFW Could not bei nitialized");
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

GLFWWindowService::~GLFWWindowService() { glfwTerminate(); }

std::shared_ptr<IWindow> GLFWWindowService::spawn_window(
    unsigned width, unsigned height, const std::string &title) {
    auto window = std::make_shared<GLFWWindow>(width, height, title);
    return _windows.emplace_back(window);
}

void GLFWWindowService::pre_render_hook() { glfwPollEvents(); }

void GLFWWindowService::post_render_hook() {}

void GLFWWindowService::setup(RendererType type) {
    switch (type) {
        case RendererType::Vulkan: {
            if (!glfwVulkanSupported())
                throw std::runtime_error("Vulkan not supported!");

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;
        }
    }
}

std::pair<unsigned int, const char **> GLFWWindowService::get_extensions()
    const {
    unsigned int count = 0;
    const char **extension_names = glfwGetRequiredInstanceExtensions(&count);
    return {count, extension_names};
}
