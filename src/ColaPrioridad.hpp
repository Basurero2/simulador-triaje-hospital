#ifndef COLA_PRIORIDAD_HPP
#define COLA_PRIORIDAD_HPP

#include <cstddef>
#include "MonticuloBinario.hpp"
#include "ListaOrdenada.hpp"

// TAD Cola de prioridad (Tabla 10). Una sola estructura sirve para las dos implementaciones:
// segun 'tipo', las funciones cp* llaman a las del montículo (m_*) o a las de la lista (l_*).
// La cola guarda punteros (void*) a los elementos y no es duena de ellos.
enum TipoCola { MONTICULO, LISTA };

typedef int (*FuncionCmp)(const void* a, const void* b);   // negativo si 'a' sale antes que 'b'

// Nodo de la lista ordenada (solo se usa cuando tipo == LISTA)
struct NodoLista {
    void* dato;
    NodoLista* siguiente;
};

struct ColaPrioridad {
    TipoCola tipo;
    FuncionCmp cmp;
    long comparaciones;   // cuantas veces se comparo (para el analisis de costos)
    int cant;             // cantidad de elementos
    void** datos;         // montículo: arreglo dinamico
    int cap;              // montículo: capacidad actual del arreglo
    NodoLista* cabeza;    // lista: primer nodo (el de mayor prioridad)
};

// Toda comparacion pasa por aqui para poder contarlas.
inline int cpAntes(ColaPrioridad* q, void* a, void* b) {
    q->comparaciones++;
    return q->cmp(a, b) < 0;
}

inline void cpIniciar(ColaPrioridad* q, TipoCola tipo, FuncionCmp cmp) {
    q->tipo = tipo;
    q->cmp = cmp;
    q->comparaciones = 0;
    q->cant = 0;
    q->datos = NULL;
    q->cap = 0;
    q->cabeza = NULL;
    if (tipo == MONTICULO) m_iniciar(q);
    else l_iniciar(q);
}

inline void cpInsertar(ColaPrioridad* q, void* x) {
    if (q->tipo == MONTICULO) m_insertar(q, x);
    else l_insertar(q, x);
}

// Devuelve NULL si la cola esta vacia.
inline void* cpExtraer(ColaPrioridad* q) {
    if (q->tipo == MONTICULO) return m_extraer(q);
    return l_extraer(q);
}

// Devuelve el primero sin sacarlo (NULL si esta vacia). Cuesta O(1) en ambas.
inline void* cpVer(const ColaPrioridad* q) {
    if (q->cant == 0) return NULL;
    if (q->tipo == MONTICULO) return q->datos[0];
    return q->cabeza->dato;
}

inline int cpVacia(const ColaPrioridad* q) { return q->cant == 0; }
inline int cpTamano(const ColaPrioridad* q) { return q->cant; }

// Reordena la cola despues de que cambiaron las prioridades de sus elementos.
inline void cpReordenar(ColaPrioridad* q) {
    if (q->tipo == MONTICULO) m_reordenar(q);
    else l_reordenar(q);
}

// Aplica f(elemento, contexto) a cada elemento. Si f cambia prioridades, hay que llamar a cpReordenar.
inline void cpParaCada(ColaPrioridad* q, void (*f)(void*, void*), void* contexto) {
    if (q->tipo == MONTICULO) m_paraCada(q, f, contexto);
    else l_paraCada(q, f, contexto);
}

// Libera la estructura (no los elementos).
inline void cpLiberar(ColaPrioridad* q) {
    if (q->tipo == MONTICULO) m_liberar(q);
    else l_liberar(q);
    q->cant = 0;
}

#endif
