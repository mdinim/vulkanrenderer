//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_SURFACE_HPP
#define VULKANENGINE_SURFACE_HPP

#include <vulkan/vulkan.h>

class IWindow;


namespace Vulkan {

class Renderer;

class Surface {
   private:
    Renderer& _renderer;
    VkSurfaceKHR _surface;

   public:
    explicit Surface(Renderer& renderer, const IWindow& window);
    virtual ~Surface();

    const VkSurfaceKHR& handle() const {
        return _surface;
    }
};
}

#endif  // VULKANENGINE_SURFACE_HPP
