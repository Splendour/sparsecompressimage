#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "extern/fmt/format.h"
#include "extern/hlslpp/hlsl++.h"

#include <vector>
#include <cassert>

using namespace hlslpp;
using std::vector;

using uint = uint32_t;
using u8 = uint8_t;


#define _USE_MATH_DEFINES
#include <math.h>

inline float GammaToLinear(float x)
{
    return (x <= 0.04045f) ? x / 12.92f : powf((x + 0.055f) / 1.055f, 2.4f);
}

inline float LinearToGamma(float x)
{
    return (x <= 0.0031308f) ? x * 12.92f : 1.055f * powf(x, 1.0f / 2.4f) - 0.055f;
}

inline float clamp(float x, float lo, float hi)
{
    return x < lo ? lo : x > hi ? hi : x;
}

inline float saturate(float x)
{
    return clamp(x, 0.f, 1.f);
}