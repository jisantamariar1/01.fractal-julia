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
    __m256 ymin = _mm256_set1_ps(y_min);

    __m256 xscale = _mm256_set1_ps(dx); //(dx, dx, dx, dx, dx, dx, dx, dx)
    __m256 yscale = _mm256_set1_ps(dy); //(dy, dy, dy, dy, dy, dy, dy, dy)

    __m256 c_real = _mm256_set1_ps(c.real());
    __m256 c_imag = _mm256_set1_ps(c.imag());

    __m256 max_norma = _mm256_set1_ps(4.0f); // El valor de 2^2 para comparar con la norma de z (4,4,4,4,4,4,4,4)
    __m256 one = _mm256_set1_ps(1.0f);       // (1, 1, 1, 1, 1, 1, 1, 1)

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j += 8)
        {
            // (i, i, i, i, i, i, i, i)
            __m256 mx = _mm256_set1_ps(i);
            // (j+7, j+6, j+5, j+4, j+3, j+2, j+1, j)
            __m256 my = _mm256_set_ps(j + 7, j + 6, j + 5, j + 4, j + 3, j + 2, j + 1, j + 0);

            // xmin+mx*xscale --> (x0, x1, x2, x3, x4, x5, x6, x7) <-- real
            // ymax-my*yscale --> (y0, y1, y2, y3, y4, y5, y6, y7) <-- imag
            __m256 cr = _mm256_add_ps(xmin, _mm256_mul_ps(mx, xscale)); // mx*xscale
            __m256 ci = _mm256_add_ps(ymin, _mm256_mul_ps(my, yscale)); // my*yscale

            // verificar si los 8 complejos (cr, ci) divergen o no (estan acotados o no)
            int iter = 1;
            __m256 mk = _mm256_set1_ps(iter);

            __m256 zr = cr; // z0.real = c.real
            __m256 zi = ci; // z0.imag = c.imag

            while (iter < max_iteraciones)
            {
                // Zn+1 = Zn^2 + c

                __m256 zr2 = _mm256_mul_ps(zr, zr);  // z^2.real = z.real * z.real
                __m256 zi2 = _mm256_mul_ps(zi, zi);  // z^2.imag = z.imag
                __m256 zrzi = _mm256_mul_ps(zr, zi); // z.real * z.imag

                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), c_real);   // z^2.real - z^2.imag + c.real
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), c_imag); // 2 * z.real * z.imag + c.imag

                //--normas
                zr2 = _mm256_mul_ps(zr, zr); // z.real^2
                zi2 = _mm256_mul_ps(zi, zi); // z.imag^2

                __m256 norma2 = _mm256_add_ps(zr2, zi2); // z.real^2 + z.imag^2 //norma^2

                __m256 mask = _mm256_cmp_ps(norma2, max_norma, _CMP_LE_OS); // compara si norma^2 < 4.0f (max_norma) y devuelve una mascara de bits

                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);

                if (_mm256_testz_ps(mask, _mm256_set1_ps(-1)))
                { // Si todos los bits de la mascara son 0, entonces todos los puntos han diverger
                    // todos los 8 complejos ya no estan acotados, salir del ciclo
                    break;
                }

                iter++;
            }

            float d[8];
            _mm256_storeu_ps(d, mk); // Almacenar el resultado de las iteraciones en un arreglo de floats
            for (int it = 0; it < 8; it++)
            {
                int index = (j + it) * width + i; // Calcular el índice del pixel en el buffer
                if (index < width * height)
                {
                    if (d[it] < max_iteraciones)
                    {
                        int color_index = (int)d[it] % PALETTE_SIZE;   // Obtener el índice de color basado en el número de iteraciones
                        pixel_buffer[index] = color_ramp[color_index]; // Asignar el color al pixel
                    }
                    else
                    {
                        pixel_buffer[index] = 0xFF000000; // Si el punto no diverge, asignar un color (
                    }
                }
            }
        }
    }
}
