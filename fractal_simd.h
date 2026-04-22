#ifndef FRACTAL_SIMD_H
#define FRACTAL_SIMD_H
#include <cstdint>

void julia_simd(float x_min, float y_min, float x_max, float y_max, int width, int height, uint32_t* pixel_buffer);


#endif