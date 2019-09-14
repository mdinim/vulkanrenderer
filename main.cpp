#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <Renderer/Vulkan/Renderer.hpp>
#include <Window/GLFWWindowService.hpp>

#include <Core/Logger/StreamLogger.hpp>

#include <chrono>
#include <iostream>

int main() {
    Core::StreamLogger logger(100, std::cout);

    GLFWWindowService window_service;

    try {
        window_service.setup(IWindowService::RendererType::Vulkan);
        auto window = window_service.spawn_window(800, 600, "Vulkan engine");
        Vulkan::Renderer renderer(window_service, window);

        renderer.initialize();

        unsigned int frames_rendered = 0;
        using namespace std::chrono_literals;
        auto measured_from = std::chrono::high_resolution_clock::now();
        while (!window->should_close()) {
            frames_rendered++;
            window_service.pre_render_hook();

            renderer.render();

            window_service.post_render_hook();

            auto time_now = std::chrono::high_resolution_clock::now();
            auto elapsed_sec = std::chrono::duration_cast<std::chrono::seconds>(
                                   time_now - measured_from)
                                   .count();
            if (elapsed_sec > 5) {
                logger.info("FPS: " +
                            std::to_string(frames_rendered / elapsed_sec));
                measured_from = std::chrono::high_resolution_clock::now();
                frames_rendered = 0;
            }
        }
    } catch (std::exception &ex) {
        logger.error(ex.what());
    }
    return 0;
}