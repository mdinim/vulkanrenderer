//
// Created by Dániel Molnár on 2019-08-02.
//

// ----- own header -----
#include <Renderer/Vulkan/Renderer.hpp>

// ----- std -----
#include <algorithm>
#include <cstring>
#include <iostream>  //todo remove
#include <map>
#include <set>
#include <utility>
#include <vector>

// ----- libraries -----
#include <Core/FileManager/BinaryFile.hpp>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ----- in-project dependencies
#include <Asset/Image.hpp>
#include <Data/Representation.hpp>
#include <Renderer/Vulkan/DescriptorSet.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <Window/IWindow.hpp>
#include <Window/IWindowService.hpp>
#include <configuration.hpp>
#include <directories.hpp>

namespace Vulkan {
const std::vector<const char*> Renderer::RequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

Renderer::Renderer(IWindowService& service,
                   std::shared_ptr<const IWindow> window)
    : _asset_manager({"", builtin_texture_dir}),
      _service(service),
      _window(std::move(window)),
      _instance(_service, "MyCorp", "CorpEngine"),
      _surface(*this, *_window),
      _physical_device(_instance, _surface),
      _logical_device(_physical_device, _surface),
      _swapchain(_surface, _physical_device, _logical_device) {
    if (auto maybe_mesh = _asset_manager.load_mesh("chalet.obj")) {
        const auto& mesh = maybe_mesh->get();

        _drawables.emplace_back(_logical_device, mesh);
        _drawables.emplace_back(_logical_device, mesh);
        _drawables.emplace_back(_logical_device, mesh);
        _drawables.emplace_back(_logical_device, mesh);
    }

    create_desc_pool_and_set();

    create_uniform_buffers();
    fill_texture("chalet.jpg");
    create_sampler();
    write_descriptor_sets();
}

Renderer::~Renderer() {
    shutdown();

    for (auto i = 0ul; i < _image_available.size(); ++i) {
        vkDestroyFence(_logical_device.handle(), _in_flight.at(i), nullptr);
        vkDestroySemaphore(_logical_device.handle(), _render_finished.at(i),
                           nullptr);
        vkDestroySemaphore(_logical_device.handle(), _image_available.at(i),
                           nullptr);
    }
    vkDestroySampler(_logical_device.handle(), _texture_sampler, nullptr);
}

void Renderer::initialize() {
    stage_drawables();
    record_command_buffers();
    create_synchronization_objects();
}

void Renderer::copy_buffer_data(Vulkan::Buffer& src, Vulkan::Buffer& dst) {
    if (!src.has_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
        !dst.has_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT))
        throw std::runtime_error("Can not execute buffer data copy!");

    auto temp_buffer = _swapchain.command_pool().allocate_temp_buffer();

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = dst.size();
    vkCmdCopyBuffer(temp_buffer.handle(), src.handle(), dst.handle(), 1,
                    &copy_region);

    temp_buffer.flush(_logical_device.graphics_queue_handle());

    vkQueueWaitIdle(_logical_device.graphics_queue_handle());
}

void Renderer::copy_image_data(Buffer& src, SubBufferDescriptor srcDesc,
                               Image& dst) {
    auto temp_buffer = _swapchain.command_pool().allocate_temp_buffer();

    dst.transition_layout(temp_buffer.handle(),
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy buffer_image_copy = {};
    buffer_image_copy.imageExtent = dst.extent();
    buffer_image_copy.bufferOffset = srcDesc.offset;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.bufferRowLength = 0;

    buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = dst.array_layers();

    vkCmdCopyBufferToImage(temp_buffer.handle(), src.handle(), dst.handle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &buffer_image_copy);
    dst.transition_layout(temp_buffer.handle(),
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    temp_buffer.flush(_logical_device.graphics_queue_handle());
}

void Renderer::fill_texture(std::string name) {
    if (auto maybe_image = _asset_manager.load_image(name)) {
        auto& image = maybe_image->get();

        _texture_image = std::make_unique<Texture2D>(
            _logical_device, image.width(), image.height());
        _texture_view = _texture_image->create_view(VK_IMAGE_ASPECT_COLOR_BIT);

        PolymorphBuffer<StagingBufferTag, StagingBufferTag> staging_buffer(
            _logical_device);

        auto texture_staging_desc =
            staging_buffer.commit_sub_buffer<StagingBufferTag>(image.size());

        staging_buffer.allocate();

        staging_buffer.transfer((void*)image.data(), texture_staging_desc);

        copy_image_data(staging_buffer, {texture_staging_desc},
                        *_texture_image);
    }
}

void Renderer::create_sampler() {
    VkSamplerCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.magFilter = VK_FILTER_LINEAR;
    create_info.minFilter = VK_FILTER_LINEAR;

    create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    create_info.anisotropyEnable = VK_TRUE;
    create_info.maxAnisotropy = 16;

    create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    create_info.unnormalizedCoordinates = VK_FALSE;

    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.mipLodBias = 0.0f;
    create_info.minLod = 0.0f;
    create_info.maxLod = 0.0f;

    if (vkCreateSampler(_logical_device.handle(), &create_info, nullptr,
                        &_texture_sampler) != VK_SUCCESS) {
        throw std::runtime_error("Could not create texture sampler");
    }
}

void Renderer::record_command_buffers() {
    for (auto i = 0ul; i < _swapchain.framebuffers().size(); ++i) {
        const auto& command_buffer = _swapchain.buffers().at(i);
        const auto& framebuffer = _swapchain.framebuffers().at(i);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error(
                "Failed to begin command buffer recording");
        }

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = _swapchain.render_pass().handle();
        render_pass_begin_info.framebuffer = framebuffer.handle();
        render_pass_begin_info.renderArea.offset = {0, 0};
        render_pass_begin_info.renderArea.extent = _swapchain.extent();

        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};
        render_pass_begin_info.clearValueCount = clear_values.size();
        render_pass_begin_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);
        for (auto j = 0u; j < _drawables.size(); ++j) {
            auto& drawable = _drawables[j];
            vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              _swapchain.graphics_pipeline().handle());
            vkCmdBindDescriptorSets(
                command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                _swapchain.graphics_pipeline().pipeline_layout(), 0, 1,
                &_new_descriptor_sets[j].handle(), 0, nullptr);
            drawable.draw(command_buffer);
        }
        vkCmdEndRenderPass(command_buffer);
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record the command buffer");
        }
    }
}
void Renderer::stage_drawables() {
    auto temp_buffer = _swapchain.command_pool().allocate_temp_buffer();
    std::map<Drawable*, Drawable::StageDesc> stage_desc_map;
    auto stage_buf =
        std::make_unique<PolymorphBuffer<StagingBufferTag>>(_logical_device);
    for (auto& drawable : _drawables) {
        stage_desc_map.emplace(&drawable, drawable.pre_stage(*stage_buf));
    }

    stage_buf->allocate();

    for (auto& drawable : _drawables) {
        drawable.stage(temp_buffer, *stage_buf, stage_desc_map.at(&drawable));
    }

    temp_buffer.flush(_logical_device.graphics_queue_handle());
}

void Renderer::create_synchronization_objects() {
    _image_available.resize(MaxFramesInFlight);
    _render_finished.resize(MaxFramesInFlight);
    _in_flight.resize(MaxFramesInFlight);

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto i = 0ul; i < _image_available.size(); ++i) {
        auto& image_available = _image_available.at(i);
        auto& render_finished = _render_finished.at(i);
        auto& in_flight = _in_flight.at(i);

        if (vkCreateSemaphore(_logical_device.handle(), &semaphore_info,
                              nullptr, &image_available) != VK_SUCCESS ||
            vkCreateSemaphore(_logical_device.handle(), &semaphore_info,
                              nullptr, &render_finished) != VK_SUCCESS ||
            vkCreateFence(_logical_device.handle(), &fence_info, nullptr,
                          &in_flight) != VK_SUCCESS) {
            throw std::runtime_error(
                "Failed to create synchronization objects for a frame");
        }
    }
}

void Renderer::create_desc_pool_and_set() {
    _new_descriptor_pool = std::make_unique<DescriptorPool>(
        _logical_device,
        std::vector{
            std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _drawables.size()},
            std::pair{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      _drawables.size()}},
        _drawables.size());

    _new_descriptor_sets = _new_descriptor_pool->allocate_sets(
        _drawables.size(),
        {_drawables.size(),
         _swapchain.graphics_pipeline().descriptor_set_layout()});
}

void Renderer::create_uniform_buffers() {
    _uniform_buffers.reserve(_drawables.size());
    for (const auto& mesh [[maybe_unused]] : _drawables) {
        _uniform_buffers.emplace_back(std::make_unique<UniformBuffer>(
            _logical_device, sizeof(UniformBufferObject)));
    }
}

void Renderer::write_descriptor_sets() {
    for (auto i = 0ul; i < _uniform_buffers.size(); ++i) {
        const auto& uniform_buffer = _uniform_buffers[i];
        auto& descriptor_set = _new_descriptor_sets[i];

        descriptor_set.write(UniformBufferObject::binding_descriptor(), 0,
                             *uniform_buffer);
        descriptor_set.write(Texture_sampler_descriptor(), 0, *_texture_image,
                             *_texture_view, _texture_sampler);
        descriptor_set.update();
    }
}

void Renderer::update_uniform_buffer(unsigned int index,
                                     uint64_t delta_time [[maybe_unused]]) {
    auto& buffer = _uniform_buffers.at(index);

    UniformBufferObject ubo = {};
    ubo.model =
        //        glm::rotate(glm::mat4(1.0), delta_time / 1000.0f *
        //        glm::radians(90.0f),
        //                    glm::vec3(0.0f, 0.0f, 1.0f));
        glm::translate(glm::rotate(glm::mat4(1.0), glm::radians(-180.f),
                                   glm::vec3(0.0f, 0.0f, 1.0f)),
                       glm::vec3(0.0f, std::pow(-1, index) * index, 0.0f));
    auto camPos = glm::vec3(2.0f, 3.0f, /*(sin(delta_time / 1000.f) + 1)*/ 1.0);

    ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.proj =
        glm::perspective(glm::radians(45.0f),
                         _swapchain.extent().width /
                             static_cast<float>(_swapchain.extent().height),
                         0.1f, 10.0f);
    ubo.proj[1][1] *= -1;  // invert Y of clip coordinate

    buffer->transfer((void*)(&ubo), sizeof(ubo));
}

void Renderer::recreate_swap_chain() {
    vkDeviceWaitIdle(_logical_device.handle());

    _swapchain.recreate();

    _uniform_buffers.clear();

    _new_descriptor_sets.clear();
    create_desc_pool_and_set();

    create_uniform_buffers();
    write_descriptor_sets();

    record_command_buffers();
}

void Renderer::resized(int width [[maybe_unused]],
                       int height [[maybe_unused]]) {
    _framebuffer_resized = true;
}

void Renderer::render(uint64_t delta_time) {
    auto& image_available = _image_available.at(_current_frame);
    auto& render_finished = _render_finished.at(_current_frame);
    auto& in_flight = _in_flight.at(_current_frame);

    // Wait until the current frame is actually submitted
    vkWaitForFences(_logical_device.handle(), 1, &in_flight, VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    auto image_index = 0u;

    // start acquiring next image
    if (auto result = _swapchain.acquireNextImage(image_index, image_available);
        result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    for (auto i = 0u; i < _drawables.size(); ++i) {
        update_uniform_buffer(i, delta_time);
    }

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // wait until the image is acquired
    VkSemaphore wait_semaphores[] = {image_available};
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &_swapchain.buffers().at(image_index);

    VkSemaphore signal_semaphores[] = {render_finished};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(_logical_device.handle(), 1, &in_flight);
    if (vkQueueSubmit(_logical_device.graphics_queue_handle(), 1, &submit_info,
                      in_flight) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command to buffer");
    }

    if (auto result = _swapchain.present(image_index, render_finished);
        result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image");
    }

    _current_frame = (_current_frame + 1) % MaxFramesInFlight;
}

void Renderer::shutdown() { vkDeviceWaitIdle(_logical_device.handle()); }
}