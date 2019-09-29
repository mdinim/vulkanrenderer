//
// Created by Dániel Molnár on 2019-09-29.
//

#pragma once
#ifndef VULKANENGINE_DRAWABLE_HPP
#define VULKANENGINE_DRAWABLE_HPP

// ----- std -----
#include <memory>

// ----- libraries -----

// ----- in-project dependencies -----
#include <Asset/Mesh.hpp>
#include <Renderer/Vulkan/Buffers.hpp>
#include <Renderer/Vulkan/Pipelines/IPipeline.hpp>

// ----- forward-decl -----
namespace Vulkan {
class LogicalDevice;
}

namespace Vulkan {
class Drawable {
   private:
    LogicalDevice& _logical_device;
    std::unique_ptr<Buffer> _buffer;
    SubBufferDescriptor _vertex_buffer_desc;
    SubBufferDescriptor _index_buffer_desc;

    const Asset::Mesh& _mesh;

   public:
    Drawable(LogicalDevice& logical_device, const Asset::Mesh& mesh);

    void stage(TempCommandBuffer& buffer, VkQueue queue);

    void draw(VkCommandBuffer command_buffer);
};
}  // namespace Vulkan

#endif  // VULKANENGINE_DRAWABLE_HPP
