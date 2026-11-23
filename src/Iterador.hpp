#ifndef ITERADOR_HPP
#define ITERADOR_HPP

#include "ColaFIFO.hpp"

// TAD Iterador (Tabla 7): recorre una ColaFIFO sin sacar a nadie.
// No se debe encolar ni desencolar mientras se recorre.
struct Iterador {
    NodoFIFO* actual;
};

inline void iteradorIniciar(Iterador* it, const ColaFIFO* c) { it->actual = c->frente; }
inline int iteradorHaySiguiente(const Iterador* it) { return it->actual != NULL; }

inline Paciente* iteradorSiguiente(Iterador* it) {
    Paciente* p = it->actual->dato;
    it->actual = it->actual->siguiente;
    return p;
}

#endif
