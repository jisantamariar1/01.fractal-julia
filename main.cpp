#include <fmt/core.h>         // Librería para formateo de texto moderno y rápido.
#include <SFML/Graphics.hpp>  // Librería principal para manejar gráficos y ventanas.
#include <complex>            // Necesario para manejar números complejos (fundamentales para fractales).
#include "fractal_serial.h"   // Incluye las funciones específicas para calcular el fractal de Julia.
//Prueba 2

// Bloque específico para Windows: permite usar funciones del sistema operativo.
#ifdef _WIN32
    #include <windows.h>
#endif

// Definimos el tamaño de la imagen. 1600x900 es una resolución HD.
#define WIDTH 1600
#define HEIGHT 900



// Límites del "plano complejo" que vamos a dibujar.
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
// --- PARÁMETROS DEL FRACTAL ---
// Cuántas veces repetiremos el cálculo antes de decidir si el punto "escapa".
int max_iteraciones = 10;

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

    sf::Texture texture({WIDTH, HEIGHT}); // Creamos una textura que se usará para mostrar el fractal.
    sf::Sprite sprite(texture); // Un sprite es un objeto que se puede dibujar en la

    sf::Font font("arial.ttf"); // Cargamos una fuente para mostrar texto (opcional).
    sf::Text text(font, "Julia Set", 24); // Creamos un objeto de texto con la fuente cargada.
    text.setFillColor(sf::Color::White); // El texto será blanco.
    text.setPosition({10, 10}); // Posicionamos el texto en la esquina superior izquierda.
    text.setStyle(sf::Text::Bold); // Hacemos el texto en negrita.

    //fps
    int frames =0;
    int fps = 0;
    sf::Clock clock; // Reloj para medir el tiempo transcurrido.
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
        //crear la textura
        julia_serial_1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        //actualizar la textura
        texture.update((const uint8_t *)pixel_buffer);
        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f){
            fps = frames;
            frames = 0;
            clock.restart();
            
        }

        
        auto msg = fmt::format("Julia Set: Iterations: {}, FPS: {}", max_iteraciones, fps);
        text.setString(msg);
        

        window.clear();   // Limpia la pantalla (la deja en negro).
        {
            window.draw(sprite);
            window.draw(text);
        }
        // (Aquí es donde más adelante dibujarás el contenido del pixel_buffer).

        window.display(); // Muestra lo que se ha dibujado en el monitor.
    }

    // LIMPIEZA DE MEMORIA:
    // Siempre que uses 'new', debes usar 'delete[]' al terminar para no desperdiciar RAM.
    delete[] pixel_buffer;

    return 0; // El programa termina con éxito.
}