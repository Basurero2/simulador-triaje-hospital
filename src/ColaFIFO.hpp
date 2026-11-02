#ifndef COLA_FIFO_HPP
#define COLA_FIFO_HPP

#include "Paciente.hpp"

struct NodoFIFO {
    Paciente dato;
    NodoFIFO* siguiente;
    NodoFIFO(const Paciente& p) : dato(p), siguiente(nullptr) {}
};

class ColaFIFO {
private:
    NodoFIFO* frente;
    NodoFIFO* fin;
    int cantidad;

public:
    ColaFIFO();
    ~ColaFIFO();

    void encolar(const Paciente& p);
    Paciente desencolar();
    bool estaVacia() const;
    int tamano() const;
};

#endif