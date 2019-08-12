//
// Created by Dániel Molnár on 2019-08-02.
//

#pragma once
#ifndef _VULKAN_ENGINE_VULKANRENDERER_HPP_
#define _VULKAN_ENGINE_VULKANRENDERER_HPP_

// ----- std -----
#include <array>
#include <memory>
#include <string_view>
#include <vector>

// ----- libraries -----
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Core/FileManager/BinaryFile.hpp>
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----
#include <Data/Representation.hpp>
#include <Renderer/IRenderer.hpp>
#include <Renderer/Vulkan/Instance.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>

// ----- forward decl -----
class IWindowService;
class IWindow;

namespace Vulkan {
class Renderer : public IRenderer {
   private:
    static constexpr int MaxFramesInFlight = 2;
    unsigned int _current_frame = 0;
    bool _framebuffer_resized = false;

    // Order of members is important to keep the order of initialization!
    IWindowService& _service;
    std::shared_ptr<const IWindow> _window;

    Instance _instance;

    Surface _surface;

    PhysicalDevice _physical_device;

    LogicalDevice _logical_device;

    Swapchain _swapchain;

    VkCommandPool _command_pool;

    VkBuffer _vertex_buffer;
    VkDeviceMemory _vertex_buffer_memory;

    std::vector<VkCommandBuffer> _command_buffers;

    std::vector<VkSemaphore> _image_available;
    std::vector<VkSemaphore> _render_finished;
    std::vector<VkFence> _in_flight;

    void create_command_pool();
    void create_vertex_buffer();
    void create_command_buffers();
    void create_synchronization_objects();

    void cleanup_swap_chain();
    void recreate_swap_chain();

    // TODO remove
    const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

   public:
    static const std::vector<const char*> RequiredExtensions;
    static constexpr const std::array<const char*, 1> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};


    Renderer(IWindowService& service, std::shared_ptr<const IWindow> window);
    virtual ~Renderer();

    const Instance& get_instance() const { return _instance; }

    void initialize() override;

    void resized(int width, int height) override;

    void render() override;

    void shutdown() override;
};
}  // namespace Vulkan

#endif
