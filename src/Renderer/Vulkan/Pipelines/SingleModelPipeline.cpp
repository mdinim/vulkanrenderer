//
// Created by Dániel Molnár on 2019-09-28.
//

// ----- own header -----
#include <Renderer/Vulkan/Pipelines/SingleModelPipeline.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Data/Representation.hpp>

namespace Vulkan {

const IPipeline::VertexBindingDescContainer&
SingleModelPipeline::BindingDescriptions() {
    static IPipeline::VertexBindingDescContainer binding_descs = {
        Vertex::binding_description()};

    return binding_descs;
}

const IPipeline::VertexAttribDescContainer&
SingleModelPipeline::AttributeDescriptions() {
    static bool initialized = false;
    static IPipeline::VertexAttribDescContainer attrib_descs;
    if (!initialized) {
        initialized = true;

        attrib_descs.reserve(Vertex::attribute_descriptions().size());
        for (const auto& attrib_desc : Vertex::attribute_descriptions()) {
            attrib_descs.emplace_back(attrib_desc);
        }
    }

    return attrib_descs;
}
}