//
// Created by Dániel Molnár on 2019-09-26.
//

#pragma once
#ifndef VULKANENGINE_MESH_HPP
#define VULKANENGINE_MESH_HPP

// ----- std -----

// ----- libraries -----
#include <assimp/Importer.hpp>

// ----- in-project dependencies -----
#include <Asset/Resource.hpp>
#include <Data/Representation.hpp>

// ----- forward-decl -----


namespace Asset {
class Mesh : public Resource {
   private:
    static Assimp::Importer Importer;

    std::string _file_name;
    bool _has_colors;
    bool _has_texture_coords;
    Vertices _vertices;
    Indices _indices;
   public:
    Mesh(ID id, std::string file_name);

    [[nodiscard]] const Vertices& vertices() const {
        return _vertices;
    }

    [[nodiscard]] const Indices& indices() const {
        return _indices;
    }
};
}

#endif  // VULKANENGINE_MESH_HPP
