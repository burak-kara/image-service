cmake_minimum_required(VERSION 3.12)
project(ImageService)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR src)

include_directories(src)
include_directories(includes/ImageMagick/include)

add_executable(ImageService ${SRC_DIR}/main.cpp)
add_executable(ImageServiceClient ${SRC_DIR}/client/main.cpp)

find_library(IMAGE_MAGICK CORE_RL_Magick++_ lib)
find_library(IMAGE_MAGICK_CORE CORE_RL_MagickCore_ lib)
find_library(IMAGE_MAGICK_WAND CORE_RL_MagickWand_ lib)

target_link_libraries(ImageService LINK_PUBLIC ${IMAGE_MAGICK} ${IMAGE_MAGICK_CORE} ${IMAGE_MAGICK_WAND})
target_link_libraries(ImageServiceClient LINK_PUBLIC ${IMAGE_MAGICK} ${IMAGE_MAGICK_CORE} ${IMAGE_MAGICK_WAND})