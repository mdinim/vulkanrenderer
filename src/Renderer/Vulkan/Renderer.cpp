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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ----- in-project dependencies
#include <Data/Representation.hpp>
#include <Renderer/Vulkan/DescriptorSet.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <Window/IWindow.hpp>
#include <Window/IWindowService.hpp>
#include <configuration.hpp>

namespace Vulkan {
const std::vector<const char*> Renderer::RequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

Renderer::Renderer(IWindowService& service,
                   std::shared_ptr<const IWindow> window)
    : _service(service),
      _window(std::move(window)),
      _instance(_service, "MyCorp", "CorpEngine"),
      _surface(*this, *_window),
      _physical_device(_instance, _surface),
      _logical_device(_physical_device, _surface),
      _swapchain(_surface, _physical_device, _logical_device) {

    _polymorph_buffer =
        std::make_unique<PolymorphBuffer<VertexBufferTag, IndexBufferTag>>(
            _logical_device);

    _vertex_buffer_desc = _polymorph_buffer->commit_sub_buffer<VertexBufferTag>(
        vertices.size() * sizeof(Vertices::value_type));

    _index_buffer_desc = _polymorph_buffer->commit_sub_buffer<IndexBufferTag>(
        indices.size() * sizeof(decltype(indices)::value_type));

    _polymorph_buffer->allocate();

    _new_descriptor_pool = std::make_unique<DescriptorPool>(
        _logical_device,
        std::vector{std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              _swapchain.images().size()}},
        _swapchain.images().size());

    _new_descriptor_sets = _new_descriptor_pool->allocate_sets(
        _swapchain.images().size(),
        {_swapchain.images().size(),
         _swapchain.graphics_pipeline().descriptor_set_layout()});

    create_uniform_buffers();
    write_descriptor_sets();
}

Renderer::~Renderer() {
    shutdown();

    for (auto i = 0u; i < _image_available.size(); ++i) {
        vkDestroyFence(_logical_device.handle(), _in_flight.at(i), nullptr);
        vkDestroySemaphore(_logical_device.handle(), _render_finished.at(i),
                           nullptr);
        vkDestroySemaphore(_logical_device.handle(), _image_available.at(i),
                           nullptr);
    }
}

void Renderer::initialize() {
    fill_buffers();
    record_command_buffers();
    create_synchronization_objects();
}

void Renderer::copy_buffer_data(
    Vulkan::Buffer& src, const std::vector<SubBufferDescriptor>& srcDescriptors,
    Vulkan::Buffer& dst,
    const std::vector<SubBufferDescriptor>& dstDescriptors) {
    if (!src.has_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
        !dst.has_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT) ||
        srcDescriptors.size() != dstDescriptors.size())
        throw std::runtime_error("Can not execute buffer data copy!");

    auto temp_buffer = _swapchain.command_pool().allocate_temp_buffer();

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(temp_buffer.handle(), &begin_info);

    std::vector<VkBufferCopy> copy_regions;
    for (auto i = 0u; i < dstDescriptors.size(); ++i) {
        VkBufferCopy copy_region = {};
        copy_region.srcOffset = srcDescriptors.at(i).offset;
        copy_region.dstOffset = dstDescriptors.at(i).offset;
        copy_region.size = dstDescriptors.at(i).size;

        copy_regions.push_back(copy_region);
    }
    vkCmdCopyBuffer(temp_buffer.handle(), src.handle(), dst.handle(),
                    copy_regions.size(), copy_regions.data());

    vkEndCommandBuffer(temp_buffer.handle());

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_buffer.handle();

    vkQueueSubmit(_logical_device.graphics_queue_handle(), 1, &submit_info,
                  VK_NULL_HANDLE);
    vkQueueWaitIdle(_logical_device.graphics_queue_handle());
}

void Renderer::copy_buffer_data(Vulkan::Buffer& src, Vulkan::Buffer& dst) {
    if (!src.has_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT) ||
        !dst.has_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT))
        throw std::runtime_error("Can not execute buffer data copy!");

    auto temp_buffer = _swapchain.command_pool().allocate_temp_buffer();

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(temp_buffer.handle(), &begin_info);

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = dst.size();
    vkCmdCopyBuffer(temp_buffer.handle(), src.handle(), dst.handle(), 1,
                    &copy_region);

    vkEndCommandBuffer(temp_buffer.handle());

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_buffer.handle();

    vkQueueSubmit(_logical_device.graphics_queue_handle(), 1, &submit_info,
                  VK_NULL_HANDLE);
    vkQueueWaitIdle(_logical_device.graphics_queue_handle());
}

void Renderer::fill_buffers() {
    PolymorphBuffer<StagingBufferTag, StagingBufferTag> staging_buffer(
        _logical_device);

    auto vertex_staging_desc =
        staging_buffer.commit_sub_buffer<StagingBufferTag>(
            _vertex_buffer_desc.size);
    auto index_staging_desc =
        staging_buffer.commit_sub_buffer<StagingBufferTag>(
            _index_buffer_desc.size);

    staging_buffer.allocate();

    staging_buffer.transfer((void*)vertices.data(), vertex_staging_desc);
    staging_buffer.transfer((void*)indices.data(), index_staging_desc);

    copy_buffer_data(staging_buffer, {vertex_staging_desc, index_staging_desc},
                     *_polymorph_buffer,
                     {_vertex_buffer_desc, _index_buffer_desc});
}

void Renderer::record_command_buffers() {
    for (auto i = 0u; i < _swapchain.framebuffers().size(); ++i) {
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

        VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                             VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _swapchain.graphics_pipeline().handle());
        VkBuffer vertex_buffers[] = {_polymorph_buffer->handle()};
        VkDeviceSize offsets[] = {_vertex_buffer_desc.offset};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(command_buffer, _polymorph_buffer->handle(),
                             _index_buffer_desc.offset, VK_INDEX_TYPE_UINT16);
        // vkCmdDraw(command_buffer, vertices.size(), 1, 0, 0, 0);
        vkCmdBindDescriptorSets(
            command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            _swapchain.graphics_pipeline().pipeline_layout(), 0, 1,
            &_new_descriptor_sets[i].handle(), 0, nullptr);
        vkCmdDrawIndexed(command_buffer, indices.size(), 1, 0, 0, 0);
        vkCmdEndRenderPass(command_buffer);
        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record the command buffer");
        }
    }
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

    for (auto i = 0u; i < _image_available.size(); ++i) {
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

void Renderer::create_uniform_buffers() {
    _uniform_buffers.reserve(_swapchain.images().size());
    for (auto swap_chain_image [[maybe_unused]] : _swapchain.images()) {
        _uniform_buffers.emplace_back(std::make_unique<UniformBuffer>(
            _logical_device, sizeof(UniformBufferObject)));
    }
}

void Renderer::write_descriptor_sets() {
    for (auto i = 0u; i < _uniform_buffers.size(); ++i) {
        const auto& uniform_buffer = _uniform_buffers[i];
        auto& descriptor_set = _new_descriptor_sets[i];

        descriptor_set.write(UniformBufferObject::binding_descriptor(), 0,
                             *uniform_buffer);
    }
}

void Renderer::update_uniform_buffer(unsigned int index, uint64_t delta_time) {
    auto& buffer = _uniform_buffers.at(index);

    UniformBufferObject ubo = {};
    ubo.model =
        glm::rotate(glm::mat4(1.0), delta_time / 1000.0f * glm::radians(90.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));

    auto camPos = glm::vec3(2.0f, 2.0f, (sin(delta_time / 1000.f) + 1));
    std::cout << camPos.z << std::endl;

    ubo.view =
        glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f),
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
    _new_descriptor_pool = std::make_unique<DescriptorPool>(
        _logical_device,
        std::vector{std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                              _swapchain.images().size()}},
        _swapchain.images().size());
    _new_descriptor_sets = _new_descriptor_pool->allocate_sets(
        _swapchain.images().size(),
        {_swapchain.images().size(),
         _swapchain.graphics_pipeline().descriptor_set_layout()});

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

    update_uniform_buffer(image_index, delta_time);

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