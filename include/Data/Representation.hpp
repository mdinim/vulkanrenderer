//
// Created by Dániel Molnár on 2019-08-06.
//

#ifndef VULKANENGINE_REPRESENTATION_HPP
#define VULKANENGINE_REPRESENTATION_HPP

// ----- std -----
#include <array>

// ----- libraries -----
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// ----- in-project dependencies -----

// ----- forward decl -----

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription binding_description() {
        VkVertexInputBindingDescription description = {};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return description;
    }

    static std::array<VkVertexInputAttributeDescription, 2>
    attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 2> descriptions = {};
        descriptions[0].binding = 0;
        descriptions[0].location = 0;
        descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        descriptions[0].offset = offsetof(Vertex, pos);

        descriptions[1].binding = 0;
        descriptions[1].location = 1;
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        descriptions[1].offset = offsetof(Vertex, color);

        return descriptions;
    }
};

#endif  // VULKANENGINE_REPRESENTATION_HPP
