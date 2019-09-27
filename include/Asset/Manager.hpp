//
// Created by Dániel Molnár on 2019-09-26.
//

#pragma once
#ifndef VULKANENGINE_MANAGER_HPP
#define VULKANENGINE_MANAGER_HPP

// ----- std -----

// ----- libraries -----
#include <Core/FileManager/FileManager.hpp>

// ----- in-project dependencies -----
#include <Asset/Image.hpp>
#include <Asset/Mesh.hpp>

// ----- forward-decl -----
namespace Asset {
class Manager {
   private:
    static ID counter;

    Core::FileManager _file_manager;

    std::map<ID, std::unique_ptr<Resource>> _resources;

   public:
    explicit Manager(std::vector<Core::FileManager::Path> search_paths);

    std::optional<std::reference_wrapper<const Image>> load_image(
        const std::string& name);

    std::optional<std::reference_wrapper<const Mesh>> load_mesh(
        const std::string& name);
};
}  // namespace Asset

#endif  // VULKANENGINE_MANAGER_HPP
