//
// Created by Dániel Molnár on 2019-08-07.
//

#include <Renderer/Vulkan/Instance.hpp>
#include <Window/IWindowService.hpp>

#include <algorithm>
#include <configuration.hpp>
#include <cstring>
#include <iostream>
#include <vector>

namespace {
std::vector<const char*> GetRequiredExtensions(const IWindowService& service) {
    auto [extension_count, service_extensions] = service.get_extensions();
    std::vector<const char*> required_extensions(
        service_extensions, service_extensions + extension_count);

    if constexpr (Configuration::EnableVulkanValidationLayers) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return required_extensions;
}

bool CheckValidationLayerSupport(
    const std::array<const char*, 1> requested_validation_layers) {
    if constexpr (Configuration::EnableVulkanValidationLayers) {
        unsigned int layer_count = 0u;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count,
                                           available_layers.data());

        for (const auto& validation_layer : requested_validation_layers) {
            if (auto it = std::find_if(
                    available_layers.begin(), available_layers.end(),
                    [&validation_layer](const auto& layer) {
                        return std::strcmp(layer.layerName, validation_layer) ==
                               0;
                    });
                it == available_layers.end()) {
                return false;
            }
        }
    }

    return true;
}

VkDebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT result = {};
    result.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    result.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    result.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    result.pUserData = nullptr;

    return result;
}

VkResult CreateDebugUtilsMessengerExt(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT& create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT& debug_messenger) {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func) {
        return func(instance, &create_info, allocator, &debug_messenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerExt(VkInstance instance,
                                   VkDebugUtilsMessengerEXT messenger,
                                   const VkAllocationCallbacks* allocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func) func(instance, messenger, allocator);
}
}  // namespace

namespace Vulkan {
Instance::Instance(const IWindowService& service, std::string application_name,
                   std::string engine_name) {
    VkApplicationInfo app_info = {};

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = application_name.data();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = engine_name.data();
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;

    const auto required_extensions = GetRequiredExtensions(service);

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if constexpr (Configuration::EnableVulkanValidationLayers) {
        if (CheckValidationLayerSupport(_validation_layers)) {
            create_info.enabledLayerCount = _validation_layers.size();
            create_info.ppEnabledLayerNames = _validation_layers.data();

            // To debug vkCreateInstance and vkDestroyInstance as well,
            // We need an additional debug messenger info
            debug_create_info = PopulateDebugMessengerCreateInfo();
            debug_create_info.pfnUserCallback = Instance::validation_callback;

            create_info.pNext = &debug_create_info;
        } else {
            throw std::runtime_error(
                "Validation layers requested, but unavailable");
        }
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instancce");
    }

    if constexpr (Configuration::EnableVulkanValidationLayers) {
        auto messenger_create_info = PopulateDebugMessengerCreateInfo();
        messenger_create_info.pfnUserCallback = Instance::validation_callback;

        if (CreateDebugUtilsMessengerExt(_instance, messenger_create_info,
                                         nullptr,
                                         _debug_messenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger");
        }
    }
}

Instance::~Instance() {
    if constexpr (Configuration::EnableVulkanValidationLayers)
        DestroyDebugUtilsMessengerExt(_instance, _debug_messenger, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

VkBool32 Instance::validation_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type [[maybe_unused]],
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data [[maybe_unused]]) {
    if constexpr (Configuration::Debug) {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cerr << "Validation layer: " << callback_data->pMessage
                          << std::endl;
                break;
            default:
                std::cout << "Validation layer: " << callback_data->pMessage
                          << std::endl;
        }
    } else {
        if (severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "validation layer: " << callback_data->pMessage
                      << std::endl;
        }
    }

    return VK_FALSE;
}

void Instance::enumerate_extensions() const {
    unsigned int count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
}
}  // namespace Vulkan
