#include <iostream>

#include <Renderer/Vulkan/Renderer.hpp>
#include <Window/GLFWWindowService.hpp>

#include <Core/Logger/StreamLogger.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <chrono>

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
        const auto start_time = measured_from;
        while (!window->should_close()) {
            auto time_now = std::chrono::high_resolution_clock::now();
            auto elapsed_sec = std::chrono::duration_cast<std::chrono::seconds>(
                                   time_now - measured_from)
                                   .count();

            frames_rendered++;
            window_service.pre_render_hook();

            renderer.render(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    time_now - start_time)
                    .count());

            window_service.post_render_hook();

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