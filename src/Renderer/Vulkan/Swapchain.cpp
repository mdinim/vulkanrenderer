//
// Created by Dániel Molnár on 2019-08-09.
//

// ----- own header -----
#include <Renderer/Vulkan/Swapchain.hpp>

// ----- std -----
#include <algorithm>
#include <vector>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/PhysicalDevice.hpp>
#include <Renderer/Vulkan/Surface.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <Window/IWindow.hpp>

namespace {
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

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                            const IWindow& window) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    auto [width, height] = window.size();
    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
}
}  // namespace

namespace Vulkan {
Swapchain::Swapchain(const Surface& surface,
                     const PhysicalDevice& physical_device,
                     const LogicalDevice& logical_device)
    : _surface(surface),
      _physical_device(physical_device),
      _logical_device(logical_device),
      _command_pool(*this) {
    create();
}

void Swapchain::create() {
    auto details = Utils::QuerySwapChainSupport(_physical_device.handle(),
                                                _surface.handle());

    auto format = ChooseSwapSurfaceFormat(details.formats);

    auto present_mode = ChooseSwapPresentMode(details.present_modes);

    _extent = ChooseSwapExtent(details.capabilities, _surface.window());
    _format = format.format;

    unsigned int image_count =
        std::clamp(details.capabilities.minImageCount + 5, 0u,
                   details.capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    create_info.surface = _surface.handle();
    create_info.minImageCount = image_count;
    create_info.imageFormat = _format;
    create_info.imageExtent = _extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.presentMode = present_mode;

    if (auto indices = Utils::FindQueueFamilies(_physical_device, _surface);
        indices.present_family != indices.graphics_family) {
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

    if (vkCreateSwapchainKHR(_logical_device.handle(), &create_info, nullptr,
                             &_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Could not create swapchain");
    }

    image_count = 0u;
    vkGetSwapchainImagesKHR(_logical_device.handle(), _swapchain, &image_count,
                            nullptr);
    _images.resize(image_count);
    vkGetSwapchainImagesKHR(_logical_device.handle(), _swapchain, &image_count,
                            _images.data());

    _image_views.reserve(image_count);
    for (const auto& image : _images) {
        _image_views.emplace_back(image, *this);
    }
    _render_pass = std::make_unique<RenderPass>(*this);
    _graphics_pipeline = std::make_unique<GraphicsPipeline>(*this);
    for (const auto& image_view : _image_views) {
        _framebuffers.emplace_back(image_view, *this);
    }
}

void Swapchain::teardown() {
    _graphics_pipeline.reset();
    _render_pass.reset();
    _framebuffers.clear();
    _image_views.clear();
    vkDestroySwapchainKHR(_logical_device.handle(), _swapchain, nullptr);
}

void Swapchain::recreate() {
    teardown();
    create();
}

Swapchain::~Swapchain() {
    vkDestroySwapchainKHR(_logical_device.handle(), _swapchain, nullptr);
}
}  // namespace Vulkan
