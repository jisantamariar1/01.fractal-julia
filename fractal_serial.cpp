#include "fractal_serial.h"
#include <complex>
extern int max_iteraciones;
extern std::complex<double> c;
uint32_t acotado_1(std::complex<double> z0) {
    /**
     * dados c, z0
     * Zn+1 = Zn^2 + c
     */
    int iter = 1;
    std::complex<double> z = z0;
    while (iter<max_iteraciones && std::abs(z) < 2.0) {
        z = z * z + c; // La fórmula del conjunto de Julia: z = z^2 + c
        iter++;
    }
    if(iter<max_iteraciones){
        //la norma>2
        return 0xFF0000FF; //color rojo
    }
    return 0xFF000000; //color negro
}

void julia_serial_1(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer) {

    double dx = (x_max - x_min) / width;   
    double dy = (y_max - y_min) / height;  

    for(int i=0;i<width;i++) {
        for(int j=0;j<height;j++) {
            // z = x+yi = (x,y)
            double x = x_min+i*dx;
            double y = y_min+j*dy;

            std::complex<double> z(x,y);

            auto color = acotado_1(z);

            pixel_buffer[j*width+i] = color;

            //acotado(z);
           
        }
    }


}
