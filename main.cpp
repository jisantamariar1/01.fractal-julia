#include <fmt/core.h>         // Librería para formateo de texto moderno y rápido.
#include <SFML/Graphics.hpp>  // Librería principal para manejar gráficos y ventanas.
#include <complex>            // Necesario para manejar números complejos (fundamentales para fractales).
//Prueba 2

// Bloque específico para Windows: permite usar funciones del sistema operativo.
#ifdef _WIN32
    #include <windows.h>
#endif

// Definimos el tamaño de la imagen. 1600x900 es una resolución HD.
#define WIDTH 1600
#define HEIGHT 900

// --- PARÁMETROS DEL FRACTAL ---
// Cuántas veces repetiremos el cálculo antes de decidir si el punto "escapa".
int max_iteraciones = 10;

// Límites del "plano complejo" que vamos a dibujar.
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

// Constante 'c' que define la forma específica del conjunto de Julia.
std::complex<double> c(-0.7, 0.27015);

// Puntero para el buffer de píxeles. 
// Cada píxel se guarda como un uint32_t (32 bits: Rojo, Verde, Azul y Alpha).
uint32_t* pixel_buffer = nullptr;

int main() {

    // RESERVA DE MEMORIA:
    // Creamos un arreglo dinámico para guardar todos los píxeles de la imagen.
    // Se usa 'new' porque un arreglo de este tamaño es demasiado grande para la memoria fija (stack).
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];

    // CREACIÓN DE VENTANA:
    // sf::VideoMode define el tamaño físico de la ventana.
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

// CÓDIGO PARA MAXIMIZAR (Solo en Windows):
#ifdef _WIN32
    // Obtenemos el identificador (handle) de la ventana que creó SFML.
    HWND hwnd = window.getNativeHandle(); 
    // Llamamos a la función de Windows para que la ventana aparezca maximizada.
    ShowWindow(hwnd, SW_MAXIMIZE);        
#endif

    // BUCLE PRINCIPAL (Game Loop):
    while (window.isOpen())
    {
        // CONTROL DE EVENTOS:
        // window.pollEvent revisa si el usuario hizo algo (teclado, ratón o cerrar).
        while (const std::optional event = window.pollEvent()) {
            // Si el evento es cerrar la ventana, el bucle termina.
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // LÓGICA DE DIBUJO:
        window.clear();   // Limpia la pantalla (la deja en negro).
        
        // (Aquí es donde más adelante dibujarás el contenido del pixel_buffer).

        window.display(); // Muestra lo que se ha dibujado en el monitor.
    }

    // LIMPIEZA DE MEMORIA:
    // Siempre que uses 'new', debes usar 'delete[]' al terminar para no desperdiciar RAM.
    delete[] pixel_buffer;

    return 0; // El programa termina con éxito.
}