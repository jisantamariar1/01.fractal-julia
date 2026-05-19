#ifndef FRACTAL_OPENMP_H
#define FRACTAL_OPENMP_H

#include <cstdint>

void julia_openmp_regiones(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);
//uint32_t acotado_openmp(double x, double y);

#endif // FRACTAL_OPENMP_H