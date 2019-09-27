//
// Created by Dániel Molnár on 2019-09-26.
//

// ----- own header -----
#include <Asset/Manager.hpp>
#include <iostream>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies

namespace Asset {

ID Manager::counter = 0;

Manager::Manager(std::vector<Core::FileManager::Path> search_paths)
    : _file_manager(std::move(search_paths)) {}

std::optional<std::reference_wrapper<const Image>> Manager::load_image(
    const std::string& name) {
    try {
        if (auto path = _file_manager.find(name)) {
            auto [it, success] = _resources.insert(
                {counter, std::make_unique<Image>(counter, *path)});

            if (success) {
                ++counter;
                return dynamic_cast<const Image&>(*it->second);
            }
        }

    } catch (std::runtime_error&) {
    }
    return {};
}

std::optional<std::reference_wrapper<const Mesh>> Manager::load_mesh(
    const std::string& name) {
    try {
        if (auto path = _file_manager.find(name)) {
            auto [it, success] = _resources.insert(
                {counter, std::make_unique<Mesh>(counter, *path)});

            if (success) {
                counter++;
                return dynamic_cast<const Mesh&>(*it->second);
            }
        }
    } catch (std::runtime_error&) {
    }
    return {};
}

}  // namespace Asset
