//
// Created by Dániel Molnár on 2019-08-12.
//

// ----- own header -----
#include <Renderer/Vulkan/Framebuffer.hpp>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies
#include <Renderer/Vulkan/ImageView.hpp>
#include <Renderer/Vulkan/LogicalDevice.hpp>
#include <Renderer/Vulkan/Swapchain.hpp>

namespace Vulkan {
Framebuffer::Framebuffer(const Vulkan::ImageView& image_view,
                         const Vulkan::Swapchain& swapchain)
    : _image_view(image_view), _swapchain(swapchain) {
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    std::array attachments = {
        image_view.handle(),
        _swapchain.render_pass().depth_image_view().handle()};
    create_info.renderPass = _swapchain.render_pass().handle();
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.width = _swapchain.extent().width;
    create_info.height = _swapchain.extent().height;
    create_info.layers = 1;

    if (vkCreateFramebuffer(_swapchain.device().handle(), &create_info, nullptr,
                            &_framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("Could not create framebuffer");
    }
}

Framebuffer::Framebuffer(Framebuffer&& other)
    : _image_view(other._image_view),
      _swapchain(other._swapchain),
      _framebuffer(other._framebuffer) {
    other._framebuffer = VK_NULL_HANDLE;
}

Framebuffer::~Framebuffer() {
    if (_framebuffer != VK_NULL_HANDLE)
        vkDestroyFramebuffer(_swapchain.device().handle(), _framebuffer,
                             nullptr);
}
}  // namespace Vulkan
