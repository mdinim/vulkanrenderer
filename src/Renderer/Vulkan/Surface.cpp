//
// Created by Dániel Molnár on 2019-08-08.
//

#include <Renderer/Vulkan/Surface.hpp>

#include <stdexcept>

#include <Window/IWindow.hpp>

#include <Renderer/Vulkan/Renderer.hpp>

namespace Vulkan {
Surface::Surface(Renderer& renderer, const IWindow& window)
    : _renderer(renderer) {
    if (auto maybe_surface = window.create_surface(_renderer)) {
        _surface = *maybe_surface;
    } else {
        throw std::runtime_error("Surface can not be created");
    }
}

Surface::~Surface() {
    vkDestroySurfaceKHR(_renderer.get_instance().handle(), _surface, nullptr);
}

}