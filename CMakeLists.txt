cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
project(
        VCNW
        VERSION 2.4.0
        LANGUAGES C CXX ASM)

cmake_policy(SET CMP0076 NEW)

find_package(CUDA REQUIRED)

find_package(Vulkan REQUIRED)
add_subdirectory(NvCodec)

add_subdirectory(src)