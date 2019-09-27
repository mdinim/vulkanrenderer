//
// Created by Dániel Molnár on 2019-09-21.
//

// ----- own header -----
#include <Asset/Image.hpp>

// ----- std -----
#include <stdexcept>
#include <utility>

// ----- libraries -----

// ----- in-project dependencies

namespace Asset {
Image::Image(ID id, std::string file_name)
    : Resource(id), _file_name(std::move(file_name)) {
    _pixels = stbi_load(_file_name.c_str(), &_width, &_height, &_channels,
                        STBI_rgb_alpha);
    if (_pixels == nullptr) {
        throw std::runtime_error("Could not load image");
    }
}

Image::~Image() { stbi_image_free(_pixels); }
}  // namespace Asset
