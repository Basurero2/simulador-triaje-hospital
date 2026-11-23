#ifndef COLA_FIFO_HPP
#define COLA_FIFO_HPP

#include <cstddef>
#include "Paciente.hpp"

// TAD ColaFIFO de capacidad limitada (Tabla 9). Lista enlazada con frente y ultimo.
// La cola guarda punteros a pacientes: no es duena de ellos.
struct NodoFIFO {
    Paciente* dato;
    NodoFIFO* siguiente;
};

struct ColaFIFO {
    NodoFIFO* frente;
    NodoFIFO* ultimo;
    int cantidad;
    int capacidad;
};

inline void colaIniciar(ColaFIFO* c, int capacidad) {
    c->frente = NULL;
    c->ultimo = NULL;
    c->cantidad = 0;
    c->capacidad = capacidad;
}

inline int colaVacia(const ColaFIFO* c) { return c->cantidad == 0; }
inline int colaLlena(const ColaFIFO* c) { return c->cantidad == c->capacidad; }
inline int colaTamano(const ColaFIFO* c) { return c->cantidad; }

// Devuelve 0 (y no modifica la cola) si esta llena; 1 si pudo encolar.
inline int colaEncolar(ColaFIFO* c, Paciente* p) {
    if (colaLlena(c)) return 0;
    NodoFIFO* nodo = new NodoFIFO;
    nodo->dato = p;
    nodo->siguiente = NULL;
    if (c->cantidad == 0) c->frente = nodo;
    else c->ultimo->siguiente = nodo;
    c->ultimo = nodo;
    c->cantidad++;
    return 1;
}

// Devuelve NULL si esta vacia.
inline Paciente* colaDesencolar(ColaFIFO* c) {
    if (colaVacia(c)) return NULL;
    NodoFIFO* nodo = c->frente;
    Paciente* p = nodo->dato;
    c->frente = nodo->siguiente;
    if (c->frente == NULL) c->ultimo = NULL;
    delete nodo;
    c->cantidad--;
    return p;
}

// Libera los nodos (no los pacientes).
inline void colaLiberar(ColaFIFO* c) {
    while (c->frente != NULL) {
        NodoFIFO* nodo = c->frente;
        c->frente = nodo->siguiente;
        delete nodo;
    }
    c->ultimo = NULL;
    c->cantidad = 0;
}

#endif
