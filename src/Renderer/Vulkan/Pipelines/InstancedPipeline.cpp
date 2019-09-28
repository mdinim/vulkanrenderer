//
// Created by Dániel Molnár on 2019-09-28.
//

// ----- own header -----
#include <Renderer/Vulkan/Pipelines/InstancedPipeline.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies

namespace Vulkan {

const IPipeline::VertexBindingDescContainer&
InstancedPipeline::BindingDescriptions() {
    static IPipeline::VertexBindingDescContainer binding_descs = {
        Vertex::binding_description(), Vertex::instance_binding_description()};

    return binding_descs;
}

const IPipeline::VertexAttribDescContainer&
InstancedPipeline::AttributeDescriptions() {
    static bool initialized = false;
    static IPipeline::VertexAttribDescContainer attrib_descs;
    if (!initialized) {
        initialized = true;

        attrib_descs.reserve(Vertex::attribute_descriptions().size() +
                             Vertex::instance_attribute_descriptions().size());
        for (const auto& attrib_desc : Vertex::attribute_descriptions()) {
            attrib_descs.emplace_back(attrib_desc);
        }
        for (const auto& attrib_desc :
             Vertex::instance_attribute_descriptions()) {
            attrib_descs.emplace_back(attrib_desc);
        }
    }

    return attrib_descs;
}

}