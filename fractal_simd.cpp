#include "fractal_simd.h"
#include "palette.h"
#include <complex>
#include <cstring>

#include <immintrin.h> // Para intrínsecos SIMD (AVX)

extern int max_iteraciones;
extern std::complex<double> c;

void julia_simd(float x_min, float y_min, float x_max, float y_max, int width, int height, uint32_t *pixel_buffer)
{
    std::memset(pixel_buffer, 0xFF000000, width * height * sizeof(uint32_t)); // Limpiamos el buffer antes de llenarlo.
    double dx = (x_max - x_min) / width;
    double dy = (y_max - y_min) / height;
    //(xmin, xmin, xmin ....) 8veces -> (-1.5,-1.5, ...)
    __m256 xmin = _mm256_set1_ps(x_min);
    //(ymax, ymax, ymax, ymax, ymax, ymax, ymax, ymax)
    //(1, 1, 1, 1, 1, 1, 1, 1)
    __m256 ymax = _mm256_set1_ps(y_max);

    __m256 xscale = _mm256_set1_ps(dx); //(dx, dx, dx, dx, dx, dx, dx, dx)
    __m256 yscale = _mm256_set1_ps(dy); //(dy, dy, dy, dy, dy, dy, dy, dy)

    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j += 8) {
            // (i, i, i, i, i, i, i, i)
            __m256 mx = _mm256_set1_ps(i);
            // (j+7, j+6, j+5, j+4, j+3, j+2, j+1, j)
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j + 0);

            // xmin+mx*xscale --> (x0, x1, x2, x3, x4, x5, x6, x7) <-- real
            // ymax-my*yscale --> (y0, y1, y2, y3, y4, y5, y6, y7) <-- imag
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, xscale)); // mx*xscale
            __m256 ci = _mm256_sub_ps(ymax, _mm256_mul_ps(my, yscale)); // my*yscale

            //verificar si los 8 complejos (cr, ci) divergen o no (estan acotados o no)
            
        }
    }
}