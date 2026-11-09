#ifndef COLAFIFO_HPP
#define COLAFIFO_HPP

#include <cstdlib>
#include "Paciente.hpp"

struct NodoFIFO {
    void* dato;
    NodoFIFO* siguiente;
};

// TAD ColaFIFO: compatible tanto con programacion procedimental estilo C (Juan Daniel)
// como con llamadas a metodos de conveniencia (Simulador).
struct ColaFIFO {
    NodoFIFO* frente;
    NodoFIFO* final;
    int cantidad;
    int capacidad;

    // Constructor C++ con capacidad por defecto (evita punteros basura al instanciar arrays)
    ColaFIFO(int cap = 1000) : frente(NULL), final(NULL), cantidad(0), capacidad(cap) {}

    // Destructor C++ para limpieza automatica
    ~ColaFIFO() {
        while (frente != NULL) {
            NodoFIFO* nodo = frente;
            frente = frente->siguiente;
            if (nodo->dato != NULL) {
                // Liberar memoria si se asigno dinamicamente
                delete (Paciente*)nodo->dato;
            }
            free(nodo);
        }
        final = NULL;
        cantidad = 0;
    }

    // Metodos miembros de conveniencia para C++
    bool estaVacia() const { return cantidad == 0; }
    bool estaLlena() const { return cantidad >= capacidad; }
    int tamano() const { return cantidad; }

    void encolar(const Paciente& p) {
        if (estaLlena()) return;
        NodoFIFO* nodo = (NodoFIFO*)malloc(sizeof(NodoFIFO));
        Paciente* copia = new Paciente(p);
        nodo->dato = (void*)copia;
        nodo->siguiente = NULL;
        if (cantidad == 0)
            frente = nodo;
        else
            final->siguiente = nodo;
        final = nodo;
        cantidad++;
    }

    Paciente desencolar() {
        if (estaVacia()) return Paciente();
        NodoFIFO* nodo = frente;
        Paciente* pPtr = (Paciente*)nodo->dato;
        Paciente res = (pPtr != NULL) ? *pPtr : Paciente();
        if (pPtr != NULL) {
            delete pPtr;
        }
        frente = nodo->siguiente;
        if (frente == NULL) final = NULL;
        free(nodo);
        cantidad--;
        return res;
    }
};

// -----------------------------------------------------------------------------
// Funciones procedimentales estilo C (TAD Juan Daniel)
// Marcadas con 'inline' para evitar errores de enlace (multiple definition)
// -----------------------------------------------------------------------------
inline void colaIniciar(ColaFIFO* c, int capacidad) {
    c->frente = NULL;
    c->final = NULL;
    c->cantidad = 0;
    c->capacidad = capacidad;
}

inline int colaVacia(ColaFIFO* c) {
    return c->cantidad == 0;
}

inline int colaLlena(ColaFIFO* c) {
    return c->cantidad == c->capacidad;
}

inline int colaTamano(ColaFIFO* c) {
    return c->cantidad;
}

inline int colaEncolar(ColaFIFO* c, void* dato) {
    if (colaLlena(c)) return 0;
    NodoFIFO* nodo = (NodoFIFO*)malloc(sizeof(NodoFIFO));
    nodo->dato = dato;
    nodo->siguiente = NULL;
    if (c->cantidad == 0)
        c->frente = nodo;
    else   
        c->final->siguiente = nodo;
    c->final = nodo;
    c->cantidad++;
    return 1;
}

inline void* colaDesencolar(ColaFIFO* c) {
    if (colaVacia(c)) return NULL;
    NodoFIFO* nodo = c->frente;
    void* dato = nodo->dato;
    c->frente = nodo->siguiente;
    if (c->frente == NULL) c->final = NULL;
    free(nodo);
    c->cantidad--;
    return dato;
}

inline void colaLiberar(ColaFIFO* c) {
    while (c->frente != NULL) {
        NodoFIFO* nodo = c->frente;
        c->frente = nodo->siguiente;
        free(nodo);
    }
    c->final = NULL;
    c->cantidad = 0;
}

#endif
