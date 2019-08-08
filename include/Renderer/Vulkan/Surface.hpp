//
// Created by Dániel Molnár on 2019-08-08.
//

#pragma once
#ifndef VULKANENGINE_SURFACE_HPP
#define VULKANENGINE_SURFACE_HPP

// ----- std -----

// ----- libraries -----
#include <vulkan/vulkan.h>

// ----- in-project dependencies -----

// ----- forward decl -----
class IWindow;
namespace Vulkan {
class Renderer;
}

namespace Vulkan {
class Surface {
   private:
    Renderer& _renderer;
    VkSurfaceKHR _surface;

   public:
    explicit Surface(Renderer& renderer, const IWindow& window);
    virtual ~Surface();

    const VkSurfaceKHR& handle() const { return _surface; }
};
}  // namespace Vulkan

#endif  // VULKANENGINE_SURFACE_HPP
