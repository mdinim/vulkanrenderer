//
// Created by Dániel Molnár on 2019-09-21.
//

#pragma once
#ifndef VULKANENGINE_ASSET_IMAGE_HPP
#define VULKANENGINE_ASSET_IMAGE_HPP

// ----- std -----
#include <string>

// ----- libraries -----
#include <stb/stb_image.h>

// ----- in-project dependencies -----
#include <Asset/Resource.hpp>

// ----- forward-decl -----

namespace Asset {
class Image : public Resource {
   private:
    std::string _file_name;

    int _width = 0;
    int _height = 0;
    int _channels = 0;
    stbi_uc* _pixels;

   public:
    explicit Image(ID id, std::string file_name);
    ~Image() override;

    [[nodiscard]] const std::byte* data() const {
        return reinterpret_cast<std::byte*>(_pixels);
    }

    [[nodiscard]] unsigned int size() const { return _width * _height * 4; }
    [[nodiscard]] int width() const { return _width; }
    [[nodiscard]] int height() const { return _height; }
};
}  // namespace Asset

#endif  // VULKANENGINE_ASSET_IMAGE_HPP
