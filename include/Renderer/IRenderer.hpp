#pragma once
#ifndef _VULKAN_ENGINE_IRENDERER_HPP_
#define _VULKAN_ENGINE_IRENDERER_HPP_

class IWindowService;
class IWindow;

class IRenderer {
   public:
    virtual void initialize(std::shared_ptr<const IWindow>) = 0;

    virtual void resized(int width, int height) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
};

#endif
