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
        Vertex::binding_description()};

    return binding_descs;
}

const IPipeline::VertexAttribDescContainer&
InstancedPipeline::AttributeDescriptions() {
    static IPipeline::VertexAttribDescContainer attrib_descs(
        Vertex::attribute_descriptions().begin(),
        Vertex::attribute_descriptions().end());

    return attrib_descs;
}
}