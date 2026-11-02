#ifndef COLA_PRIORIDAD_HPP
#define COLA_PRIORIDAD_HPP

#include "Paciente.hpp"

class ColaPrioridad {
public:
    virtual ~ColaPrioridad() {}

    // Firmas obligatorias que deben implementar Heap y Lista
    virtual void insertar(const Paciente& p) = 0;
    virtual Paciente extraer() = 0;
    virtual bool estaVacia() const = 0;
    virtual int tamano() const = 0;
};

#endif