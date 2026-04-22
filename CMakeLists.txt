# 1. Especifica la versión mínima de CMake necesaria.
# La versión 3.16 asegura compatibilidad con características modernas de C++ y librerías.
cmake_minimum_required(VERSION 3.16)

# 2. Define el nombre del proyecto.
# Esto ayuda a organizar los archivos internos que genera Visual Studio Code.
project(FractalJulia)

# 3. Busca la librería 'fmt' en tu sistema (gestionada por vcpkg).
# CONFIG: Indica que use el archivo de configuración oficial de la librería.
# REQUIRED: Si no encuentra la librería, el proceso se detiene con un error.
find_package(fmt CONFIG REQUIRED)

# 4. Busca la librería SFML y los módulos específicos que necesitas.
# System: Funciones base (tiempo, hilos).
# Graphics: Para dibujar el fractal (texturas, sprites).
# Window: Para crear y gestionar la ventana donde se verá el fractal.
find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

# 5. Define la creación del programa ejecutable (.exe).
# 'fractal-julia' es el nombre que tendrá el archivo final.
# Aquí listamos todos los archivos .cpp que forman parte del programa:
# main.cpp tiene la ventana y fractal_serial.cpp tendrá la lógica del fractal.
add_executable(fractal-julia
 main.cpp
 fractal_serial.cpp
 palette.cpp
 fractal_simd.cpp
)

# 6. "Enlaza" o conecta las librerías con tu ejecutable.
# Esto le dice al compilador: "Para que fractal-julia funcione, pega estas piezas".
# PRIVATE significa que estas dependencias son internas de tu proyecto.
target_link_libraries(fractal-julia PRIVATE
    fmt::fmt          # Conecta las funciones de texto de fmt.
    SFML::System      # Conecta el núcleo de SFML.
    SFML::Graphics    # Conecta las herramientas de dibujo.
    SFML::Window      # Conecta la gestión de la ventana.
)