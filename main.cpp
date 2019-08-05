#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <Renderer/VulkanRenderer.hpp>
#include <Window/GLFWWindowService.hpp>

#include <iostream>

int main() {
  GLFWWindowService window_service;
  VulkanRenderer renderer(window_service);

  //try {
    auto window = window_service.spawn_window(800, 600, "Vulkan engine");

    renderer.initialize(window);

    while (!window->should_close()) {
      window_service.pre_render_hook();

      renderer.render();

      window_service.post_render_hook();
    }
  //} catch (std::exception &ex) {
  //  std::cout << ex.what() << std::endl;
  //}
  return 0;
}