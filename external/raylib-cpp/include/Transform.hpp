#ifndef RAYLIB_CPP_INCLUDE_TRANSFORM_HPP_
#define RAYLIB_CPP_INCLUDE_TRANSFORM_HPP_
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace raylib
{
    struct Transform3D
    {
        raylib::Vector3 translation = { 0.0f, 0.0f, 0.0f };
        raylib::Quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
        raylib::Vector3 scale = { 1.0f, 1.0f, 1.0f };
    };

    struct Transform2D
    {
        raylib::Vector2 translation = { 0.0f, 0.0f };
        float rotation = 0.0f;
        raylib::Vector2 scale = { 1.0f, 1.0f };
    };
}

#endif // RAYLIB_CPP_INCLUDE_TRANSFORM_HPP_