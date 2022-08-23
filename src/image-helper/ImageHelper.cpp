#pragma once
#include "ImageHelper.h"

ImageHelper::ImageHelper(char *argv) {
    InitializeMagick(argv);
};

auto ImageHelper::getImage(const std::string &url) {
    return Image(url);
}

auto ImageHelper::setImage(const Blob &blob) {
    return Image(blob);
}

void ImageHelper::writeImage(Image &image, const std::string &path) {
    image.write(path);
}

void ImageHelper::crop(Image &image, const std::string &size) {
    image.crop(Geometry(size));
}

void ImageHelper::crop(Image &image, const Size &size, const Size &offset) {
    image.crop(Geometry(size.width, size.height, offset.width, offset.height));
}

void ImageHelper::grayscale(Image &image) {
    image.type(GrayscaleType);
}

void ImageHelper::resize(Image &image, const std::string  &size) {
    image.resize(Geometry(size));
}

void ImageHelper::rotate(Image &image, std::int16_t degrees) {
    image.rotate(degrees);
}
