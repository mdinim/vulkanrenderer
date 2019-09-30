//
// Created by Dániel Molnár on 2019-09-29.
//

// ----- own header -----
#include <Renderer/Vulkan/Drawable.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/CommandPool.hpp>

namespace Vulkan {
Drawable::Drawable(Vulkan::LogicalDevice& logical_device,
                   const Asset::Mesh& mesh)
    : _logical_device(logical_device), _mesh(mesh) {
    auto buffer =
        std::make_unique<PolymorphBuffer<VertexBufferTag, IndexBufferTag>>(
            _logical_device);

    _vertex_buffer_desc =
        buffer->commit_sub_buffer<VertexBufferTag>(mesh.vertex_data_size());
    _index_buffer_desc =
        buffer->commit_sub_buffer<IndexBufferTag>(mesh.index_data_size());

    buffer->allocate();

    _buffer = std::move(buffer);
}

void Drawable::transfer(Vulkan::TempCommandBuffer& command_buffer, VkQueue queue) {
    auto stage_buf =
        std::make_unique<PolymorphBuffer<StagingBufferTag>>(_logical_device);
    auto desc = pre_stage(*stage_buf);
    stage_buf->allocate();
    stage(command_buffer, *stage_buf, desc);

    command_buffer.flush(queue);
}

Drawable::StageDesc Drawable::pre_stage(
    PolymorphBuffer<StagingBufferTag>& stage) {
    StageDesc desc{{0, 0, 0}, {0, 0, 0}};

    desc.vert_desc =
        stage.commit_sub_buffer<StagingBufferTag>(_vertex_buffer_desc.size);
    desc.ind_desc =
        stage.commit_sub_buffer<StagingBufferTag>(_index_buffer_desc.size);

    return desc;
}

void Drawable::stage(TempCommandBuffer& command_buffer,
                    PolymorphBuffer<StagingBufferTag>& stage,
                    const Drawable::StageDesc& desc) {
    stage.transfer((void*)_mesh.vertices().data(), desc.vert_desc);
    stage.transfer((void*)_mesh.indices().data(), desc.ind_desc);
    stage.copy_to(command_buffer, *_buffer, {desc.vert_desc, desc.ind_desc},
                  {_vertex_buffer_desc, _index_buffer_desc});
}

void Drawable::draw(VkCommandBuffer command_buffer) {
    VkBuffer vertex_buffers[] = {_buffer->handle()};
    VkDeviceSize offsets[] = {_vertex_buffer_desc.offset};
    vkCmdBindVertexBuffers(command_buffer,
                           Vertex::binding_description().binding, 1,
                           vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, _buffer->handle(),
                         _index_buffer_desc.offset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffer, _mesh.indices().size(), 1, 0, 0, 0);
}

}