cmake_minimum_required(VERSION 3.16)
project(triaje_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Sin indicar nada, se compila optimizado (-O2), como el comando de consola del informe
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(triaje_gui
    gui/main_gui.cpp
    gui/VentanaPrincipal.cpp
    gui/GraficoBarras.cpp
    src/Paciente.cpp
    src/MonticuloBinario.cpp
    src/ListaOrdenada.cpp
    src/Simulador.cpp
)
target_include_directories(triaje_gui PRIVATE src gui)
target_link_libraries(triaje_gui PRIVATE Qt6::Widgets)

# Copia la carpeta data junto al .exe para que encuentre el CSV
add_custom_command(TARGET triaje_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:triaje_gui>/data")