//
// Created by Dániel Molnár on 2019-08-02.
//

#pragma once
#ifndef _VULKAN_ENGINE_VULKANRENDERER_HPP_
#define _VULKAN_ENGINE_VULKANRENDERER_HPP_

// ----- std -----
#include <array>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

// ----- libraries -----
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Core/FileManager/BinaryFile.hpp>
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----
#include <Asset/Manager.hpp>
#include <Renderer/IRenderer.hpp>
#include <Renderer/Vulkan/Buffers.hpp>
#include <Renderer/Vulkan/DescriptorPool.hpp>
#include <Renderer/Vulkan/Images.hpp>
#include <Renderer/Vulkan/Instance.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <Renderer/Vulkan/Drawable.hpp>

// ----- forward decl -----
class IWindowService;
class IWindow;

namespace Vulkan {
class Renderer : public IRenderer {
   private:
    static constexpr int MaxFramesInFlight = 2;
    unsigned int _current_frame = 0;
    bool _framebuffer_resized = false;

    Asset::Manager _asset_manager;

    // Order of members is important to keep the order of initialization!
    IWindowService& _service;
    std::shared_ptr<const IWindow> _window;

    Instance _instance;

    Surface _surface;

    PhysicalDevice _physical_device;

    LogicalDevice _logical_device;

    Swapchain _swapchain;

    std::unique_ptr<DescriptorPool> _new_descriptor_pool;
    std::vector<DescriptorSet> _new_descriptor_sets;

    std::unique_ptr<Image> _texture_image;
    std::unique_ptr<ImageView> _texture_view;
    VkSampler _texture_sampler;

    std::vector<std::unique_ptr<Buffer>> _uniform_buffers;

    std::vector<VkSemaphore> _image_available;
    std::vector<VkSemaphore> _render_finished;
    std::vector<VkFence> _in_flight;

    std::vector<Drawable> _drawables;

    void copy_buffer_data(Buffer& src, Buffer& dst);

    void copy_image_data(Buffer& src, SubBufferDescriptor srcDescriptors,
                         Image& dst);

    void fill_texture(std::string name);
    void stage_drawables();
    void create_sampler();
    void record_command_buffers();
    void create_synchronization_objects();

    void create_uniform_buffers();
    void write_descriptor_sets();

    void recreate_swap_chain();

   public:
    static const std::vector<const char*> RequiredExtensions;
    static constexpr const std::array<const char*, 1> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    Renderer(IWindowService& service, std::shared_ptr<const IWindow> window);
    virtual ~Renderer();

    [[nodiscard]] const Instance& get_instance() const { return _instance; }

    void initialize() override;

    void resized(int width, int height) override;

    void render(uint64_t delta_time) override;

    void shutdown() override;
    void update_uniform_buffer(unsigned int index, uint64_t delta_time);
    void create_desc_pool_and_set();
};
}  // namespace Vulkan

#endif
