#pragma once
#ifndef _VULKAN_ENGINE_VULKANRENDERER_HPP_
#define _VULKAN_ENGINE_VULKANRENDERER_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Core/FileManager/BinaryFile.hpp>
#include <Core/FileManager/FileManager.hpp>

#include <array>
#include <memory>
#include <string_view>
#include <vector>

#include <Renderer/IRenderer.hpp>

class IWindowService;
class IWindow;

class VulkanRenderer : public IRenderer {
   private:
    static constexpr int MaxFramesInFlight = 2;
    unsigned int _current_frame = 0;
    bool _framebuffer_resized = false;

    VkInstance _instance;
    Core::FileManager _shader_manager;

    VkDebugUtilsMessengerEXT _debug_messenger;

    VkSurfaceKHR _surface;

    VkPhysicalDevice _physical_device = VK_NULL_HANDLE;

    VkDevice _logical_device;
    VkQueue _graphics_queue;
    VkQueue _present_queue;

    VkSwapchainKHR _swap_chain;

    std::vector<VkImage> _swap_chain_images;
    std::vector<VkImageView> _swap_chain_image_views;

    VkFormat _swap_chain_format;
    VkExtent2D _swap_chain_extent;

    VkRenderPass _render_pass;
    VkPipelineLayout _pipeline_layout;

    VkPipeline _graphics_pipeline;

    VkCommandPool _command_pool;
    std::vector<VkCommandBuffer> _command_buffers;

    std::vector<VkFramebuffer> _swap_chain_framebuffers;

    std::vector<VkSemaphore> _image_available;
    std::vector<VkSemaphore> _render_finished;
    std::vector<VkFence> _in_flight;

    IWindowService& _service;
    std::shared_ptr<const IWindow> _window;
    static constexpr const std::array<const char*, 1> _validation_layers = {
        "VK_LAYER_KHRONOS_validation"};

    void create_instance();
    void setup_debug_messenger();
    void create_surface();
    void pick_physical_device();
    void create_logical_device();
    void create_swap_chain();
    void create_swap_chain_image_views();
    void create_render_pass();
    void create_graphics_pipeline();
    VkShaderModule create_shader_module(
        const Core::BinaryFile::ByteSequence& code);
    void create_framebuffers();
    void create_command_pool();
    void create_command_buffers();
    void create_synchronization_objects();

    void cleanup_swap_chain();
    void recreate_swap_chain();

    bool check_validation_layer_support() const;
    std::vector<const char*> get_required_extensions() const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL validation_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data);

   public:
    static const std::vector<const char*> RequiredExtensions;

    VulkanRenderer(IWindowService& service);
    virtual ~VulkanRenderer();

    const VkInstance& get_instance() const { return _instance; }

    void initialize(std::shared_ptr<const IWindow> window) override;

    void resized(int width, int height) override;

    void render() override;

    void shutdown() override;
};

#endif
