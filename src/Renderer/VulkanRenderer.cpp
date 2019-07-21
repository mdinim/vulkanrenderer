#include <Renderer/VulkanRenderer.hpp>
#include <Window/IWindow.hpp>
#include <Window/IWindowService.hpp>
#include <configuration.hpp>

#include <iostream>  //todo remove
#include <map>
#include <set>
#include <vector>

namespace {
struct QueueFamily {
    std::optional<unsigned int> graphics_family;
    std::optional<unsigned int> present_family;

    explicit operator bool() const { return graphics_family && present_family; }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

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

QueueFamily FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    static std::map<std::pair<VkPhysicalDevice, VkSurfaceKHR>, QueueFamily>
        queue_family_cache;

    // early bail
    if (queue_family_cache[{device, surface}])
        return queue_family_cache.at({device, surface});

    auto& queue_family = queue_family_cache[{device, surface}];
    auto queue_family_count = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families.data());
    {
        VkBool32 present_supported = false;
        auto i = 0;
        for (const auto& found_family : queue_families) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                 &present_supported);
            if (found_family.queueCount > 0 &&
                (found_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                queue_family.graphics_family = i;
            }
            if (found_family.queueCount > 0 && present_supported) {
                queue_family.present_family = i;
            }

            if (queue_family) break;

            i++;
        }
    }

    return queue_family;
}

bool CheckExtensionSupport(VkPhysicalDevice device) {
    unsigned int extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                         nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);

    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                         available_extensions.data());

    std::set<std::string> remaining_extensions(
        VulkanRenderer::RequiredExtensions.begin(),
        VulkanRenderer::RequiredExtensions.end());

    for (const auto& extension : available_extensions) {
        if (remaining_extensions.find(extension.extensionName) !=
            remaining_extensions.end()) {
            remaining_extensions.erase(extension.extensionName);
        }
    }

    return remaining_extensions.empty();
}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    unsigned int format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         nullptr);

    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         details.formats.data());

    unsigned int present_mode_count = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &present_mode_count, nullptr);

    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, details.present_modes.data());

    return details;
}

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    const auto SwapChainDetails = QuerySwapChainSupport(device, surface);

    const bool SwapChainAdequate = !SwapChainDetails.formats.empty() &&
                                   !SwapChainDetails.present_modes.empty();

    return static_cast<bool>(FindQueueFamilies(device, surface)) &&
           CheckExtensionSupport(device) && SwapChainAdequate;
}

int RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);

    if (!IsDeviceSuitable(device, surface)) return 0;

    auto queue_family = FindQueueFamilies(device, surface);

    int score = 0;

    if (queue_family.present_family == queue_family.graphics_family)
        score += 10;

    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    score += device_properties.limits.maxImageDimension2D;
    score += device_properties.limits.maxImageDimension3D;

    return score;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& available_formats) {
    for (const auto& format : available_formats) {
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
            format.format == VK_FORMAT_B8G8R8_UNORM) {
            return format;
        }
    }
    return available_formats.front();
}

VkPresentModeKHR ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& available_present_modes) {
    for (const auto& present_mode : available_present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {800, 600};  // Todo handle resize

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
}

}  // namespace

const std::vector<const char*> VulkanRenderer::RequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VulkanRenderer::VulkanRenderer(IWindowService& service) : _service(service) {
    _service.setup(*this);
}

VulkanRenderer::~VulkanRenderer() {
    for(const auto& image_view : _swap_chain_image_views) {
        vkDestroyImageView(_logical_device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(_logical_device, _swap_chain, nullptr);
    vkDestroyDevice(_logical_device, nullptr);
    DestroyDebugUtilsMessengerExt(_instance, _debug_messenger, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

void VulkanRenderer::initialize(IWindow& window) {
    create_instance();
    setup_debug_messenger();
    create_surface(window);
    pick_physical_device();
    create_logical_device();
    create_swap_chain();
    create_swap_chain_image_views();
    create_graphics_pipeline();
}

void VulkanRenderer::create_instance() {
    VkApplicationInfo app_info = {};

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "MyCorp";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "OwnEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    const auto required_extensions = get_required_extensions();

    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    // To ensure it does not get destroyed before vkCreateInstancce call
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    if (Configuration::EnableVulkanValidationLayers) {
        if (check_validation_layer_support()) {
            create_info.enabledLayerCount = _validation_layers.size();
            create_info.ppEnabledLayerNames = _validation_layers.data();

            debug_create_info = PopulateDebugMessengerCreateInfo();
            debug_create_info.pfnUserCallback =
                VulkanRenderer::validation_callback;

            create_info.pNext = &debug_create_info;
        } else {
            throw std::runtime_error(
                "Validation layers requested, "
                "but not available!");
        }
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan Instance!");
    }

    unsigned int count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

    std::cout << " === AVAILABLE EXTENSIONS ===" << std::endl;
    for (const auto& extension : extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
}

void VulkanRenderer::setup_debug_messenger() {
    auto create_info = PopulateDebugMessengerCreateInfo();
    create_info.pfnUserCallback = VulkanRenderer::validation_callback;

    if (CreateDebugUtilsMessengerExt(_instance, create_info, nullptr,
                                     _debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}

void VulkanRenderer::create_surface(IWindow& window) {
    if (auto maybe_surface = window.create_surface(*this);
        maybe_surface.has_value()) {
        _surface = maybe_surface.value();
    } else {
        throw std::runtime_error("Surface can not be created");
    }
}

void VulkanRenderer::pick_physical_device() {
    auto device_count = 0u;
    vkEnumeratePhysicalDevices(_instance, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(_instance, &device_count, devices.data());

    int best_score = 0;
    for (const auto& device : devices) {
        auto score = RateDeviceSuitability(device, _surface);

        // device is not suitable
        if (score == 0) continue;

        if (score > best_score) {
            _physical_device = device;
        }
    }

    if (_physical_device == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find suitable GPU!");
}

void VulkanRenderer::create_logical_device() {
    auto indices = FindQueueFamilies(_physical_device, _surface);

    const std::set<unsigned int> families = {*indices.graphics_family,
                                             *indices.present_family};

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (const auto& family : families) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = family;
        queue_create_info.queueCount = 1;

        const float queue_pritority = 1.0;
        queue_create_info.pQueuePriorities = &queue_pritority;

        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures required_features = {};

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();

    create_info.pEnabledFeatures = &required_features;

    create_info.ppEnabledExtensionNames =
        VulkanRenderer::RequiredExtensions.data();
    create_info.enabledExtensionCount =
        VulkanRenderer::RequiredExtensions.size();

    if constexpr (Configuration::EnableVulkanValidationLayers) {
        create_info.ppEnabledLayerNames = _validation_layers.data();
        create_info.enabledLayerCount = _validation_layers.size();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(_physical_device, &create_info, nullptr,
                       &_logical_device) != VK_SUCCESS) {
        throw std::runtime_error("Could not create logical device!");
    }

    vkGetDeviceQueue(_logical_device, indices.graphics_family.value(), 0,
                     &_graphics_queue);
    vkGetDeviceQueue(_logical_device, indices.present_family.value(), 0,
                     &_present_queue);
}

void VulkanRenderer::create_swap_chain() {
    auto details = QuerySwapChainSupport(_physical_device, _surface);

    auto format = ChooseSwapSurfaceFormat(details.formats);

    auto present_mode = ChooseSwapPresentMode(details.present_modes);

    _swap_chain_extent = ChooseSwapExtent(details.capabilities);
    _swap_chain_format = format.format;

    unsigned int image_count =
        std::clamp(details.capabilities.minImageCount + 5, 0u,
                   details.capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR create_info = {};

    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = _surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageExtent = _swap_chain_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    create_info.presentMode = present_mode;

    auto indices = FindQueueFamilies(_physical_device, _surface);

    if (indices.present_family != indices.graphics_family) {
        unsigned int indices_array[] = {indices.graphics_family.value(),
                                        indices.present_family.value()};

        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = indices_array;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.clipped = true;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_logical_device, &create_info, nullptr,
                             &_swap_chain) != VK_SUCCESS) {
        throw std::runtime_error("Could not create swap chain!");
    }

    vkGetSwapchainImagesKHR(_logical_device, _swap_chain, &image_count,
                            nullptr);
    _swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(_logical_device, _swap_chain, &image_count,
                            _swap_chain_images.data());
}

void VulkanRenderer::create_swap_chain_image_views() {
    _swap_chain_image_views.resize(_swap_chain_images.size());

    for (auto i = 0u; i < _swap_chain_images.size(); ++i) {
        const auto& image = _swap_chain_images[i];
        auto& image_view = _swap_chain_image_views[i];

        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        create_info.image = image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = _swap_chain_format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;

        if (vkCreateImageView(_logical_device, &create_info, nullptr,
                              &image_view) != VK_SUCCESS) {
            throw std::runtime_error("Could not create image view!");
        }
    }
}

void VulkanRenderer::create_graphics_pipeline() {

}

std::vector<const char*> VulkanRenderer::get_required_extensions() const {
    auto [extension_count, service_extensions] = _service.get_extensions();
    std::vector<const char*> required_extensions(
        service_extensions, service_extensions + extension_count);

    if constexpr (Configuration::EnableVulkanValidationLayers) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return required_extensions;
}

bool VulkanRenderer::check_validation_layer_support() const {
    if constexpr (Configuration::EnableVulkanValidationLayers) {
        unsigned int layer_count = 0u;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count,
                                           available_layers.data());

        for (const auto& validation_layer : _validation_layers) {
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

void VulkanRenderer::render() {}

VkBool32 VulkanRenderer::validation_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity [[maybe_unused]],
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
