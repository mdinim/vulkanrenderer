//
// Created by Dániel Molnár on 2019-08-10.
//

#pragma once
#ifndef VULKANENGINE_PIPELINE_HPP
#define VULKANENGINE_PIPELINE_HPP

// ----- std -----
#include <iostream>  // todo remove

// ----- libraries -----
#include <vulkan/vulkan_core.h>
#include <Core/FileManager/BinaryFile.hpp>
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----
#include <Data/Representation.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Pipelines/IPipeline.hpp>
#include <Renderer/Vulkan/Shaders/FragmentShader.hpp>
#include <Renderer/Vulkan/Shaders/VertexShader.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <Renderer/Vulkan/Utils.hpp>
#include <directories.hpp>

// ----- forward decl -----

namespace Vulkan {

template <class SpecializedPipeline>
class Pipeline : public IPipeline {
   private:
    const Swapchain& _swapchain;

    VkPipelineLayout _pipeline_layout;
    VkPipeline _pipeline;
    std::vector<VkDescriptorSetLayout> _layouts;

    void create() {
        auto shaders = SpecializedPipeline::Shaders(_swapchain.device());
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        std::transform(
            shaders.begin(), shaders.end(), std::back_inserter(shader_stages),
            [](const auto& shader) {
                VkPipelineShaderStageCreateInfo create_info = {};

                create_info.sType =
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

                create_info.module = shader->module();
                create_info.stage = shader->stage();
                create_info.pName = shader->entry_point();

                return create_info;
            });

        VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
        vertex_input_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        vertex_input_state_info.vertexBindingDescriptionCount =
            SpecializedPipeline::BindingDescriptions().size();
        vertex_input_state_info.pVertexBindingDescriptions =
            SpecializedPipeline::BindingDescriptions().data();
        vertex_input_state_info.vertexAttributeDescriptionCount =
            SpecializedPipeline::AttributeDescriptions().size();
        vertex_input_state_info.pVertexAttributeDescriptions =
            SpecializedPipeline::AttributeDescriptions().data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
        input_assembly_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_swapchain.extent().width);
        viewport.height = static_cast<float>(_swapchain.extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = _swapchain.extent();

        VkPipelineViewportStateCreateInfo viewport_state_info = {};
        viewport_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

        viewport_state_info.viewportCount = 1;
        viewport_state_info.pViewports = &viewport;
        viewport_state_info.scissorCount = 1;
        viewport_state_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterization_state_info = {};
        rasterization_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        rasterization_state_info.depthClampEnable = VK_FALSE;
        rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_info.lineWidth = 1.0f;
        rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization_state_info.depthBiasEnable = VK_FALSE;
        rasterization_state_info.depthBiasConstantFactor = 0.0f;
        rasterization_state_info.depthBiasClamp = 0.0f;
        rasterization_state_info.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisample_state_info = {};
        multisample_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

        multisample_state_info.sampleShadingEnable = VK_FALSE;
        multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample_state_info.minSampleShading = 1.0f;
        multisample_state_info.pSampleMask = nullptr;
        multisample_state_info.alphaToCoverageEnable = VK_FALSE;
        multisample_state_info.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment = {};
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blend_state_info = {};
        color_blend_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

        color_blend_state_info.logicOpEnable = VK_FALSE;
        color_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
        color_blend_state_info.attachmentCount = 1;
        color_blend_state_info.pAttachments = &color_blend_attachment;
        color_blend_state_info.blendConstants[0] = 0.0f;
        color_blend_state_info.blendConstants[1] = 0.0f;
        color_blend_state_info.blendConstants[2] = 0.0f;
        color_blend_state_info.blendConstants[3] = 0.0f;

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info = {};
        depth_stencil_state_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_state_info.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil_state_info.depthTestEnable = VK_TRUE;
        depth_stencil_state_info.depthWriteEnable = VK_TRUE;
        depth_stencil_state_info.minDepthBounds = 0.0f;
        depth_stencil_state_info.maxDepthBounds = 1.0f;
        depth_stencil_state_info.stencilTestEnable = VK_FALSE;
        depth_stencil_state_info.front = {};
        depth_stencil_state_info.back = {};

        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        pipeline_layout_info.setLayoutCount = _layouts.size();
        pipeline_layout_info.pSetLayouts = _layouts.data();
        pipeline_layout_info.pushConstantRangeCount =
            SpecializedPipeline::PushConstants().size();
        pipeline_layout_info.pPushConstantRanges =
            SpecializedPipeline::PushConstants().data();

        if (vkCreatePipelineLayout(_swapchain.device().handle(),
                                   &pipeline_layout_info, nullptr,
                                   &_pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("Could not create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        create_info.stageCount = shader_stages.size();
        create_info.pStages = shader_stages.data();
        create_info.pVertexInputState = &vertex_input_state_info;
        create_info.pInputAssemblyState = &input_assembly_info;
        create_info.pViewportState = &viewport_state_info;
        create_info.pRasterizationState = &rasterization_state_info;
        create_info.pMultisampleState = &multisample_state_info;
        create_info.pDepthStencilState = &depth_stencil_state_info;
        create_info.pColorBlendState = &color_blend_state_info;
        create_info.pDynamicState = nullptr;
        create_info.layout = _pipeline_layout;
        create_info.renderPass = _swapchain.render_pass().handle();
        create_info.subpass = 0;
        create_info.basePipelineHandle = VK_NULL_HANDLE;
        create_info.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(_swapchain.device().handle(),
                                      VK_NULL_HANDLE, 1, &create_info, nullptr,
                                      &_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Could not create graphics pipeline");
        }
    }

    void teardown() {
        vkDestroyPipelineLayout(_swapchain.device().handle(), _pipeline_layout,
                                nullptr);
        vkDestroyPipeline(_swapchain.device().handle(), _pipeline, nullptr);
    }

   public:
    Pipeline(const Swapchain& swapchain,
             const std::vector<VkDescriptorSetLayout>& layouts)
        : _swapchain(swapchain), _layouts(layouts) {
        create();
    }

    void recreate() override {
        teardown();
        create();
    }

    virtual ~Pipeline() { teardown(); }

    [[nodiscard]] const VkPipeline& handle() const override {
        return _pipeline;
    };

    // TODO remove
    const VkPipelineLayout& pipeline_layout() const override {
        return _pipeline_layout;
    }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_PIPELINE_HPP
