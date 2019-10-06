//
// Created by Dániel Molnár on 2019-08-06.
//

#ifndef VULKANENGINE_REPRESENTATION_HPP
#define VULKANENGINE_REPRESENTATION_HPP

// ----- std -----
#include <array>
#include <vector>

// ----- libraries -----
#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// ----- in-project dependencies -----

// ----- forward decl -----

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex;

    static constexpr VkVertexInputBindingDescription binding_description() {
        VkVertexInputBindingDescription description = {};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return description;
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 3>
    attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 3> descriptions = {};
        descriptions[0].binding = binding_description().binding;
        descriptions[0].location = 0;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[0].offset = offsetof(Vertex, pos);

        descriptions[1].binding = binding_description().binding;
        descriptions[1].location = 1;
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[1].offset = offsetof(Vertex, color);

        descriptions[2].binding = binding_description().binding;
        descriptions[2].location = 2;
        descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        descriptions[2].offset = offsetof(Vertex, tex);

        return descriptions;
    }

    static constexpr VkVertexInputBindingDescription
    instance_binding_description() {
        VkVertexInputBindingDescription description = {};
        description.binding = 1;
        description.stride = sizeof(glm::vec3);
        description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return description;
    }

    static constexpr std::array<VkVertexInputAttributeDescription, 1>
    instance_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 1> descriptions = {};
        descriptions[0].binding = instance_binding_description().binding;
        descriptions[0].location = 3;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[0].offset = 0;

        return descriptions;
    }
};
using Vertices = std::vector<Vertex>;
using Indices = std::vector<uint32_t>;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;

    static constexpr VkDescriptorSetLayoutBinding binding_descriptor() {
        VkDescriptorSetLayoutBinding ubo_layout_binding = {};
        ubo_layout_binding.binding = 0;
        ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_binding.pImmutableSamplers = nullptr;

        return ubo_layout_binding;
    }
};

constexpr VkDescriptorSetLayoutBinding Texture_sampler_descriptor() {
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_binding.pImmutableSamplers = nullptr;

    return layout_binding;
}

#endif  // VULKANENGINE_REPRESENTATION_HPP
