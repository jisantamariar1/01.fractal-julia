#include "fractal_simd.h"
#include "palette.h"
#include <complex>
#include <cstring>

extern int max_iteraciones;
extern std::complex<double> c;

void julia_simd(float x_min, float y_min, float x_max, float y_max, int width, int height, uint32_t* pixel_buffer) {
    std::memset(pixel_buffer, 0xFF000000, width * height * sizeof(uint32_t)); // Limpiamos el buffer antes de llenarlo.
}