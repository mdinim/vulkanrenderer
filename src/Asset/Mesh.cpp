//
// Created by Dániel Molnár on 2019-09-26.
//

// ----- own header -----
#include <Asset/Mesh.hpp>

// ----- std -----
#include <stdexcept>

// ----- libraries -----
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Data/Representation.hpp>

// ----- in-project dependencies

namespace Asset {

Assimp::Importer Mesh::Importer;

Mesh::Mesh(ID id, std::string file_name)
    : Resource(id), _file_name(std::move(file_name)) {
    auto scene = Importer.ReadFile(
        _file_name, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                       aiProcess_SortByPType | aiProcess_FlipUVs);
    if (scene->mNumMeshes == 0) {
        throw std::runtime_error("Mesh could not be loaded!");
    }

    auto mesh = scene->mMeshes[0];

    _has_colors = mesh->HasVertexColors(0);
    _has_texture_coords = mesh->HasTextureCoords(0);
    for (auto i = 0u; i < mesh->mNumVertices; ++i) {
        auto [x, y, z] = mesh->mVertices[i];
        auto [r, g, b, a] =
            _has_colors ? mesh->mColors[0][i] : aiColor4D{.5, .5, .5, .5};
        auto [u, v, w] = _has_texture_coords ? mesh->mTextureCoords[0][i]
                                          : aiVector3D{0, 0, 0};

        Vertex vert = {{x, y, z}, {r, g, b}, {u, v}};
        _vertices.emplace_back(std::move(vert));
    }

    for (auto i = 0u; i < mesh->mNumFaces; ++i) {
        auto& face = mesh->mFaces[i];
        for (auto j = 0u; j < face.mNumIndices; ++j) {
            _indices.emplace_back(face.mIndices[j]);
        }
    }

    Importer.FreeScene();
}

}