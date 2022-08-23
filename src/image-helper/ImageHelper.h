#pragma once
#include <iostream>
#include <Magick++.h>
#include "size/Size.h"

#ifndef IMAGE_SERVICE_IMAGEHELPER_H
#define IMAGE_SERVICE_IMAGEHELPER_H

using namespace Magick;

class ImageHelper {
public:
    explicit ImageHelper(char *argv);

    static auto getImage(const std::string &url);

    static auto setImage(const Blob &blob);

    static void writeImage(Image &image, const std::string &path);

    static void crop(Image &image, const std::string &size);

    static void crop(Image &image, const Size &size, const Size &offset);

    static void grayscale(Image &image);

    static void resize(Image &image, const std::string &size);

    static void rotate(Image &image, std::int16_t degrees);
};


#endif //IMAGE_SERVICE_IMAGEHELPER_H
