cmake_minimum_required(VERSION 3.24)
project(PictureIntoVideo)

set(CMAKE_CXX_STANDARD 11)

add_executable(PictureIntoVideo main.cpp bmp_reader.cpp bmp_reader.h video_editor.cpp video_editor.h)
