// src/Iterador.hpp - TAD Iterador sobre una ColaFIFO
// No encolar ni desencolar mientras se recorre.
#ifndef ITERADOR_HPP
#define ITERADOR_HPP
 
#include "ColaFIFO.hpp"
 
struct Iterador {
    NodoFIFO* actual;
};
 
inline void iterIniciar(Iterador* it, ColaFIFO* c) {
    it->actual = c->frente;
}
 
inline int iterHaySiguiente(Iterador* it) {
    return it->actual != NULL;
}
 
// Devuelve el elemento actual y avanza. NULL si ya no quedan.
inline void* iterSiguiente(Iterador* it) {
    if (it->actual == NULL) return NULL;
    void* dato = it->actual->dato;
    it->actual = it->actual->siguiente;
    return dato;
}
 
#endif