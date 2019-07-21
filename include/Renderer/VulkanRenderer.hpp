#pragma once
#ifndef _VULKAN_ENGINE_VULKANRENDERER_HPP_
#define _VULKAN_ENGINE_VULKANRENDERER_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <memory>
#include <string_view>
#include <vector>

#include <Renderer/IRenderer.hpp>

class IWindowService;
class IWindow;

class VulkanRenderer : public IRenderer {
   private:
    VkInstance _instance;

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

    IWindowService& _service;
    static constexpr const std::array<const char*, 1> _validation_layers = {
        "VK_LAYER_KHRONOS_validation"};


    void create_instance();
    void setup_debug_messenger();
    void create_surface(IWindow& window);
    void pick_physical_device();
    void create_logical_device();
    void create_swap_chain();
    void create_swap_chain_image_views();
    void create_graphics_pipeline();

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

    void initialize(IWindow& window) override;

    void render() override;
};

#endif
