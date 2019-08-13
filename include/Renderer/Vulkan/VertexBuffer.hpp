//
// Created by Dániel Molnár on 2019-08-13.
//

#pragma once
#ifndef VULKANENGINE_VERTEXBUFFER_HPP
#define VULKANENGINE_VERTEXBUFFER_HPP

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies -----
#include <Renderer/Vulkan/Buffer.hpp>

// ----- forward-decl -----

namespace Vulkan {
class VertexBuffer : public Buffer {
   private:
   public:
    VertexBuffer(const PhysicalDevice& physical_device,
                 const LogicalDevice& logical_device, VkDeviceSize buffer_size,
                 VkSharingMode sharing_mode);

    ~VertexBuffer() override = default;
};
}  // namespace Vulkan

#endif  // VULKANENGINE_VERTEXBUFFER_HPP
