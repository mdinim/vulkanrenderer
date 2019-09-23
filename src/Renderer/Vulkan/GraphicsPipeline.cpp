//
// Created by Dániel Molnár on 2019-08-10.
//

// ----- own header -----
#include <Renderer/Vulkan/GraphicsPipeline.hpp>

// ----- std -----

// ----- libraries -----
#include <Core/FileManager/BinaryFile.hpp>

// ----- in-project dependencies
#include <Data/Representation.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>
#include <directories.hpp>

namespace {

VkShaderModule CreateShaderModule(const Vulkan::LogicalDevice& logical_device,
                                  const Core::BinaryFile::ByteSequence& code) {
    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(logical_device.handle(), &create_info, nullptr,
                             &module) != VK_SUCCESS) {
        throw std::runtime_error("Could not creat shader module!");
    }

    return module;
}
}  // namespace

namespace Vulkan {
GraphicsPipeline::GraphicsPipeline(const Swapchain& swapchain)
    : _shader_manager({std::filesystem::current_path(), builtin_shader_dir}),
      _swapchain(swapchain) {
    auto create_shader_module = [&](const std::string& shader_filename) {
        if (auto shader_file = _shader_manager.binary_file(shader_filename)) {
            if (auto shader_code = shader_file->read()) {
                return CreateShaderModule(_swapchain.device(),
                                          shader_code.value());
            } else {
                throw std::runtime_error("Shader file " +
                                         shader_file->path().string() +
                                         " could not be read");
            }
        } else {
            throw std::runtime_error("Built-in vertex shader not found");
        }
    };

    VkShaderModule vertex_shader_module =
        create_shader_module("shader_vert.spv");
    VkShaderModule fragment_shader_module =
        create_shader_module("shader_frag.spv");

    auto destroy_shaders = [&]() {
        vkDestroyShaderModule(_swapchain.device().handle(),
                              vertex_shader_module, nullptr);
        vkDestroyShaderModule(_swapchain.device().handle(),
                              fragment_shader_module, nullptr);
    };

    struct DestroyShaders {
        std::function<void()> _functor;
        DestroyShaders(std::function<void()> functor)
            : _functor(std::move(functor)) {}

        ~DestroyShaders() { _functor(); }

    } destroyer(destroy_shaders);

    VkPipelineShaderStageCreateInfo vertex_create_info = {};
    vertex_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    vertex_create_info.module = vertex_shader_module;
    vertex_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo fragment_create_info = {};
    fragment_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    fragment_create_info.module = fragment_shader_module;
    fragment_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_create_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_create_info,
                                                       fragment_create_info};

    auto binding_description = Vertex::binding_description();
    auto attribute_descriptions = Vertex::attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
    vertex_input_state_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertex_input_state_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_info.vertexAttributeDescriptionCount =
        attribute_descriptions.size();
    vertex_input_state_info.pVertexAttributeDescriptions =
        attribute_descriptions.data();

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

    auto ubo_layout_binding_descriptor =
        UniformBufferObject::binding_descriptor();

    std::array bindings = {ubo_layout_binding_descriptor,
                           texture_sampler_descriptor()};

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
    descriptor_set_layout_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.bindingCount = bindings.size();
    descriptor_set_layout_info.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_swapchain.device().handle(),
                                    &descriptor_set_layout_info, nullptr,
                                    &_descriptor_set_layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create descriptor sett layout");
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(_swapchain.device().handle(),
                               &pipeline_layout_info, nullptr,
                               &_pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    create_info.stageCount = 2;
    create_info.pStages = shader_stages;
    create_info.pVertexInputState = &vertex_input_state_info;
    create_info.pInputAssemblyState = &input_assembly_info;
    create_info.pViewportState = &viewport_state_info;
    create_info.pRasterizationState = &rasterization_state_info;
    create_info.pMultisampleState = &multisample_state_info;
    create_info.pDepthStencilState = nullptr;
    create_info.pColorBlendState = &color_blend_state_info;
    create_info.pDynamicState = nullptr;
    create_info.layout = _pipeline_layout;
    create_info.renderPass = _swapchain.render_pass().handle();
    create_info.subpass = 0;
    create_info.basePipelineHandle = VK_NULL_HANDLE;
    create_info.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(_swapchain.device().handle(), VK_NULL_HANDLE,
                                  1, &create_info, nullptr,
                                  &_graphics_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Could not create graphics pipeline");
    }
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyDescriptorSetLayout(_swapchain.device().handle(),
                                 _descriptor_set_layout, nullptr);

    vkDestroyPipelineLayout(_swapchain.device().handle(), _pipeline_layout,
                            nullptr);
    vkDestroyPipeline(_swapchain.device().handle(), _graphics_pipeline,
                      nullptr);
}
// namespace Vulkan
}