//
// Created by Dániel Molnár on 2019-08-10.
//

// ----- own header -----
#include <Renderer/Vulkan/RenderPass.hpp>

// ----- std -----

// ----- libraries -----
#include <Core/FileManager/BinaryFile.hpp>
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>

namespace Vulkan {
RenderPass::RenderPass(const Vulkan::Swapchain& swapchain)
    : _swapchain(swapchain) {
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = _swapchain.format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    create_info.attachmentCount = 1;
    create_info.pAttachments = &color_attachment;
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    // Implicit subpass BEFORE the render pass begins
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    // Wait on the color attachment to be available (swapchain finished reading)
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    // Wait with reading and writing
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

    create_info.dependencyCount = 1;
    create_info.pDependencies = &dependency;

    if (vkCreateRenderPass(_swapchain.device().handle(), &create_info, nullptr,
                           &_render_pass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render pass");
    }
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(_swapchain.device().handle(), _render_pass, nullptr);
}
}