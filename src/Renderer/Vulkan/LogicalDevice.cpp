//
// Created by Dániel Molnár on 2019-08-08.
//

// ----- own header -----
#include <Renderer/Vulkan/LogicalDevice.hpp>

// ----- std -----
#include <set>
#include <vector>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Renderer.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <configuration.hpp>

// ----- forward decl -----

namespace Vulkan {
LogicalDevice::LogicalDevice(PhysicalDevice& physical_device,
                             Surface& surface) {
    auto indices = Vulkan::Utils::FindQueueFamilies(physical_device.handle(),
                                                    surface.handle());

    const std::set<unsigned int> families = {*indices.graphics_family,
                                             *indices.present_family};

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (const auto& family : families) {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = family;
        queue_create_info.queueCount = 1;

        const float queue_priority = 1.0;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures required_features = {};

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &required_features;

    create_info.enabledExtensionCount = Renderer::RequiredExtensions.size();
    create_info.ppEnabledExtensionNames = Renderer::RequiredExtensions.data();

    if constexpr (Configuration::EnableVulkanValidationLayers) {
        create_info.enabledLayerCount = Renderer::ValidationLayers.size();
        create_info.ppEnabledLayerNames = Renderer::ValidationLayers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physical_device.handle(), &create_info, nullptr,
                       &_device) != VK_SUCCESS) {
        throw std::runtime_error("Could not create logical device");
    }

    vkGetDeviceQueue(_device, *indices.graphics_family, 0, &_graphics_queue);
    vkGetDeviceQueue(_device, *indices.present_family, 0, &_present_queue);
}
LogicalDevice::~LogicalDevice() { vkDestroyDevice(_device, nullptr); }
}  // namespace Vulkan
