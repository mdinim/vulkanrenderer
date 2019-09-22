//
// Created by Dániel Molnár on 2019-09-21.
//

// ----- own header -----
#include <Asset/Image.hpp>
#include <utility>

// ----- std -----

// ----- libraries -----

// ----- in-project dependencies

namespace Asset {
Image::Image(std::string file_name) : _file_name(std::move(file_name)) {
    _pixels = stbi_load(_file_name.c_str(), &_width, &_height, &_channels,
                        STBI_rgb_alpha);
}

Image::~Image() { stbi_image_free(_pixels); }
}  // namespace Asset
