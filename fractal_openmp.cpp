#include "fractal_openmp.h"
#include "palette.h"
#include <omp.h> // Librería de OpenMP para paralelización.

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


