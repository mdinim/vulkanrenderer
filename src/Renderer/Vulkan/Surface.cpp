//
// Created by Dániel Molnár on 2019-08-08.
//

// ----- own header -----
#include <Renderer/Vulkan/Surface.hpp>

// ----- std -----
#include <stdexcept>

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/Renderer.hpp>
#include <Window/IWindow.hpp>

namespace Vulkan {
Surface::Surface(Renderer& renderer, const IWindow& window)
    : _renderer(renderer), _window(window) {
    if (auto maybe_surface = _window.create_surface(_renderer)) {
        _surface = *maybe_surface;
    } else {
        throw std::runtime_error("Surface can not be created");
    }
}

Surface::~Surface() {
    vkDestroySurfaceKHR(_renderer.get_instance().handle(), _surface, nullptr);
}

}