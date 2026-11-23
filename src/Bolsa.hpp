#ifndef BOLSA_HPP
#define BOLSA_HPP

#include <cstddef>

// TAD Bolsa (Tabla 8): multiconjunto de claves enteras como lista de pares (clave, veces).
struct NodoBolsa {
    int clave;
    int veces;
    NodoBolsa* siguiente;
};

struct Bolsa {
    NodoBolsa* primero;
};

inline void bolsaIniciar(Bolsa* b) { b->primero = NULL; }

inline void bolsaAgregar(Bolsa* b, int clave) {
    NodoBolsa* nodo = b->primero;
    while (nodo != NULL && nodo->clave != clave) nodo = nodo->siguiente;
    if (nodo != NULL) {
        nodo->veces++;                       // la clave ya existia
    } else {
        NodoBolsa* nuevo = new NodoBolsa;    // clave nueva: se agrega al inicio
        nuevo->clave = clave;
        nuevo->veces = 1;
        nuevo->siguiente = b->primero;
        b->primero = nuevo;
    }
}

inline int bolsaContar(const Bolsa* b, int clave) {
    NodoBolsa* nodo = b->primero;
    while (nodo != NULL && nodo->clave != clave) nodo = nodo->siguiente;
    return (nodo == NULL) ? 0 : nodo->veces;
}

inline void bolsaLiberar(Bolsa* b) {
    while (b->primero != NULL) {
        NodoBolsa* nodo = b->primero;
        b->primero = nodo->siguiente;
        delete nodo;
    }
}

#endif
