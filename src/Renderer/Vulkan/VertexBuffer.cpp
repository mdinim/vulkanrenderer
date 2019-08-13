//
// Created by Dániel Molnár on 2019-08-13.
//

// ----- own header -----
#include <Renderer/Vulkan/VertexBuffer.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies

namespace Vulkan {
VertexBuffer::VertexBuffer(const Vulkan::PhysicalDevice& physical_device,
                           const Vulkan::LogicalDevice& logical_device,
                           VkDeviceSize buffer_size, VkSharingMode sharing_mode)
    : Buffer(physical_device, logical_device, buffer_size,
             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sharing_mode) {}

}  // namespace Vulkan
