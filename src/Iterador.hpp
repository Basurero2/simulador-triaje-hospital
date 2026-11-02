#ifndef ITERADOR_HPP
#define ITERADOR_HPP

#include "Paciente.hpp"
#include <vector>

class IteradorPaciente {
private:
    const std::vector<Paciente>& coleccion;
    size_t posicion;

public:
    IteradorPaciente(const std::vector<Paciente>& col) : coleccion(col), posicion(0) {}
    bool tieneSiguiente() const { return posicion < coleccion.size(); }
    Paciente siguiente() { return coleccion[posicion++]; }
    void reiniciar() { posicion = 0; }
};

#endif