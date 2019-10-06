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
class Texture2D;
class LogicalDevice;
}  // namespace Vulkan

namespace Vulkan {
class Drawable {
   private:
    LogicalDevice& _logical_device;
    std::unique_ptr<Buffer> _buffer;
    SubBufferDescriptor _vertex_buffer_desc;
    SubBufferDescriptor _index_buffer_desc;

    const Asset::Mesh& _mesh;
    Texture2D* _texture;

   public:
    struct StageDesc {
        SubBufferDescriptor vert_desc;
        SubBufferDescriptor ind_desc;
    };

    Drawable(LogicalDevice& logical_device, const Asset::Mesh& mesh);

    void transfer(TempCommandBuffer& command_buffer, VkQueue queue);
    StageDesc pre_stage(PolymorphBuffer<StagingBufferTag>& stage);
    void stage(TempCommandBuffer& command_buffer,
               PolymorphBuffer<StagingBufferTag>& stage,
               const Drawable::StageDesc& desc);

    void attach_texture(Texture2D* texture);

    Texture2D* texture() const;

    void draw(VkCommandBuffer command_buffer);
};
}  // namespace Vulkan

#endif  // VULKANENGINE_DRAWABLE_HPP
