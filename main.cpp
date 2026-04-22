#include <fmt/core.h>         // Para formatear el texto del HUD (FPS e iteraciones) de forma eficiente.
#include <SFML/Graphics.hpp>  // Motor gráfico para crear la ventana y manejar la textura de los píxeles.
#include <complex>            // Necesario para definir la constante 'c' del fractal.
#include "fractal_serial.h"   // Nuestras funciones de cálculo de Julia.

// Si estamos en Windows, incluimos su API para poder maximizar la ventana manualmente.
#ifdef _WIN32
    #include <windows.h>
#endif

// Definimos la resolución de nuestra área de dibujo.
#define WIDTH 1600
#define HEIGHT 900

// Definimos los límites del plano complejo donde queremos "mirar".
double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

// Parámetros dinámicos: pueden cambiar mientras el programa corre.
int max_iteraciones = 10;
std::complex<double> c(-0.7, 0.27015); // La semilla que da forma al fractal de Julia.

// El puntero que administrará nuestro bloque gigante de píxeles en la RAM.
uint32_t* pixel_buffer = nullptr;

int main() {
    // 1. ASIGNACIÓN DE MEMORIA (HEAP):
    // Reservamos espacio para 1.44 millones de píxeles (1600*900).
    pixel_buffer = new uint32_t[WIDTH * HEIGHT];

    // 2. CONFIGURACIÓN DE LA VENTANA:
    // Creamos la ventana de SFML con el título "Julia Set".
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

// Si es Windows, forzamos que la ventana se abra maximizada usando el handle nativo.
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle(); 
    ShowWindow(hwnd, SW_MAXIMIZE);        
#endif

    // 3. OBJETOS GRÁFICOS DE SFML:
    // La 'texture' vive en la VRAM (Tarjeta de video) y recibirá los datos del pixel_buffer.
    sf::Texture texture({WIDTH, HEIGHT}); 
    // El 'sprite' es el objeto que permite "dibujar" la textura en la ventana.
    sf::Sprite sprite(texture); 

    // Cargamos una fuente y configuramos el texto que mostrará los FPS.
    sf::Font font("arial.ttf"); 
    sf::Text text(font, "Julia Set", 24); 
    text.setFillColor(sf::Color::White); 
    text.setPosition({10, 10}); 
    text.setStyle(sf::Text::Bold); 

    std::string options = "Options: [1] Serial 1 [2] Serial 2 | Up/Down: Change iterations";
    sf::Text textOptions(font, options, 20);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getSize().y - 40}); // Posicionar en la parte inferior de la ventana.

    // Variables para medir el rendimiento (Frames Per Second).
    int frames = 0;
    int fps = 0;
    sf::Clock clock; 

    // 4. BUCLE PRINCIPAL (Game Loop): Se ejecuta hasta que se cierre la ventana.
    while (window.isOpen())
    {
        // A. PROCESAR EVENTOS: Entrada del usuario.
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close(); // Cerrar el programa.
            else if(event->is<sf::Event::KeyReleased>()) {
                auto evt = event->getIf<sf::Event::KeyReleased>();
                // Controlamos las iteraciones con las flechas del teclado.
                switch(evt->scancode) {
                    case sf::Keyboard::Scan::Up:
                        max_iteraciones += 10; // Más detalle.
                        break;
                    case sf::Keyboard::Scan::Down:
                        max_iteraciones -= 10; // Menos detalle (más rápido).
                        if(max_iteraciones < 10) max_iteraciones = 10;
                        break;
                }
            }
        }

        // B. CÁLCULO DEL FRACTAL (Lógica pesada):
        // Llamamos a nuestra función para que llene el pixel_buffer con colores.
        julia_serial_2(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);
        //julia_serial_1(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, pixel_buffer);

        // C. ACTUALIZAR TEXTURA:
        // Enviamos los datos calculados en la CPU hacia la Tarjeta de Video.
        texture.update((const uint8_t *)pixel_buffer);
        frames++;

        // D. CÁLCULO DE FPS: Cada vez que pase 1 segundo, actualizamos el contador.
        if (clock.getElapsedTime().asSeconds() >= 1.0f){
            fps = frames;
            frames = 0;
            clock.restart();
        }

        // E. ACTUALIZAR HUD: Formateamos el mensaje de texto.
        auto msg = fmt::format("Julia Set: Iterations: {}, FPS: {}", max_iteraciones, fps);
        text.setString(msg);

        // F. RENDERIZADO:
        window.clear();      // Limpiar la pantalla (borrar el frame anterior).
        window.draw(sprite); // Dibujar el fractal (la textura).
        window.draw(text);   // Dibujar el contador de FPS encima.
        window.draw(textOptions); // Dibujar las opciones de control.
        window.display();    // Intercambiar buffers para mostrar el dibujo en el monitor.
    }

    // 5. LIMPIEZA:
    // Muy importante para evitar fugas de memoria al cerrar el programa.
    delete[] pixel_buffer;

    return 0; 
}