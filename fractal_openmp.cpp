#include "fractal_openmp.h"
#include "palette.h"
#include <omp.h> // Librería de OpenMP para paralelización.
#include <immintrin.h> // Para intrínsecos SIMD (AVX)

#include <complex>

extern int max_iteraciones;
extern std::complex<double> c;
// Aquí hacemos la matemática de complejos "a mano" para ganar velocidad.
uint32_t acotado_openmp(double x, double y) {
    int iter = 1;
    double zr = x; // Parte real de z.
    double zi = y; // Parte imaginaria de z.

    // OPTIMIZACIÓN: (zr*zr + zi*zi) < 4.0 es equivalente a abs(z) < 2.0.
    // Se usa 4.0 porque así evitamos calcular la raíz cuadrada interna de abs().
    while (iter < max_iteraciones && (zr * zr + zi * zi) < 4.0) {
        
        // Aplicamos binomio al cuadrado: (a + bi)^2 = (a^2 - b^2) + (2ab)i.
        double dr = zr * zr - zi * zi + c.real(); // Nueva parte real.
        double di = 2.0 * zr * zi + c.imag();     // Nueva parte imaginaria.
        
        zr = dr; // Actualizamos para la siguiente iteración.
        zi = di;
        iter++;
    }

    if(iter < max_iteraciones){
        int index = iter % PALETTE_SIZE; // Usamos el contador de iteraciones para elegir un color de la paleta.
        return color_ramp[index]; // Devolvemos el color correspondiente de la paleta.
        //return 0xFF0000FF; // Rojo si escapa.
    }
    return 0xFF000000; // Negro si no escapa.
}
void julia_openmp_regiones(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    double dx = (x_max - x_min) / width;   
    double dy = (y_max - y_min) / height; 
    
    #pragma omp parallel
    {
        int thread_count = omp_get_num_threads();
        int thread_id = omp_get_thread_num();

        int delta = std::ceil(width * 1.0 / thread_count);

        int start = thread_id * delta;
        int end = (thread_id+1) * delta;

        if(thread_id == thread_count-1){
            end = width;
        }
        

        for(int i = start; i < end; i++){
            for(int j = 0; j < height; j++){


            double x = x_min + i * dx;
            double y = y_min + j * dy;

            // Llamamos a la versión optimizada que recibe doubles directamente.
            auto color = acotado_openmp(x, y);

            pixel_buffer[j * width + i] = color;
            }
        }

    }
}

void julia_openmp_for(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    double dx = (x_max - x_min) / width;   
    double dy = (y_max - y_min) / height; 
    
    #pragma omp parallel for
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){

            double x = x_min + i * dx;
            double y = y_min + j * dy;

            // Llamamos a la versión optimizada que recibe doubles directamente.
            auto color = acotado_openmp(x, y);

            pixel_buffer[j * width + i] = color;

        }
    }

    
}

void julia_openmp_for_simd(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

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
    
    #pragma omp parallel for simd
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


    



