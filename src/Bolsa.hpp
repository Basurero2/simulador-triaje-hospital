// src/Bolsa.hpp - TAD Bolsa (multiconjunto de claves enteras)
// Extension opcional: cuenta pacientes por diagnostico.
#ifndef BOLSA_HPP
#define BOLSA_HPP

#include <cstdlib>   // malloc, free, NULL

struct NodoBolsa {
    int clave;
    int veces;
    NodoBolsa* siguiente;
};

struct Bolsa {
    NodoBolsa* primero;
};

inline void bolsaIniciar(Bolsa* b) {
    b->primero = NULL;
}

inline void bolsaAgregar(Bolsa* b, int clave) {
    NodoBolsa* nodo = b->primero;
    while (nodo != NULL && nodo->clave != clave) {
        nodo = nodo->siguiente;
    }
    if (nodo != NULL) {
        nodo->veces++;
    } else {
        NodoBolsa* nuevo = (NodoBolsa*) malloc(sizeof(NodoBolsa));
        nuevo->clave = clave;
        nuevo->veces = 1;
        nuevo->siguiente = b->primero;
        b->primero = nuevo;
    }
}

// Cuantas veces se agrego la clave (0 si nunca).
inline int bolsaContar(Bolsa* b, int clave) {
    NodoBolsa* nodo = b->primero;
    while (nodo != NULL && nodo->clave != clave) {
        nodo = nodo->siguiente;
    }
    if (nodo == NULL) return 0;
    return nodo->veces;
}

inline void bolsaLiberar(Bolsa* b) {
    while (b->primero != NULL) {
        NodoBolsa* nodo = b->primero;
        b->primero = nodo->siguiente;
        free(nodo);
    }
}

#endif