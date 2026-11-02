#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

#include <string>
#include "ColaPrioridad.hpp"

class Simulador {
private:
    int tiempoActual;

public:
    Simulador();
    void cargarDatosDesdeCSV(const std::string& rutaCSV);
    void ejecutar(ColaPrioridad& colaTriaje);
    void mostrarReporteEstadistico() const;
};

#endif