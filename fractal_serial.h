// 1. LAS GUARDAS DE INCLUSIÓN (Include Guards)
// Si NO está definido el símbolo FRACTAL_SERIAL_H...
#ifndef FRACTAL_SERIAL_H 
// ...entonces defínelo ahora mismo.
#define FRACTAL_SERIAL_H 

// 2. LIBRERÍAS EXTERNAS
// Incluye tipos de enteros con tamaño exacto (como uint32_t para los colores).
#include <cstdint> 

// 3. DECLARACIÓN DE FUNCIONES (Prototipos)

/**
 * julia_serial_1: Calcula el conjunto de Julia usando la librería <complex> de C++.
 */
void julia_serial_1(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);

/**
 * julia_serial_2: Hace lo mismo que la 1, pero sin usar <complex> (calcula la matemática manualmente).
 * Se suele usar para comparar si esta versión es más rápida que la que usa la librería estándar.
 */
void julia_serial_2(double x_min, double y_min, double x_max, double y_max, uint32_t width, uint32_t height, uint32_t* pixel_buffer);

// 4. FIN DE LA GUARDA
// Indica el final del bloque protegido por el #ifndef.
#endif