#include "ColaPrioridad.hpp"

// Lista enlazada ordenada: la cabeza siempre es el elemento de mayor prioridad.

// Enlaza un nodo ya creado en su posicion correcta (despues de los que no pierden contra el).
static void enlazar(ColaPrioridad* q, NodoLista* nodo) {
    if (q->cabeza == NULL || cpAntes(q, nodo->dato, q->cabeza->dato)) {
        nodo->siguiente = q->cabeza;               // va al inicio
        q->cabeza = nodo;
    } else {
        NodoLista* actual = q->cabeza;
        while (actual->siguiente != NULL && !cpAntes(q, nodo->dato, actual->siguiente->dato)) {
            actual = actual->siguiente;            // avanzar mientras el siguiente no pierda contra el
        }
        nodo->siguiente = actual->siguiente;
        actual->siguiente = nodo;
    }
}

void l_iniciar(ColaPrioridad* q) {
    q->cabeza = NULL;
    q->cant = 0;
}

void l_insertar(ColaPrioridad* q, void* x) {
    NodoLista* nodo = new NodoLista;
    nodo->dato = x;
    nodo->siguiente = NULL;
    enlazar(q, nodo);
    q->cant++;
}

void* l_extraer(ColaPrioridad* q) {
    if (q->cabeza == NULL) return NULL;
    NodoLista* nodo = q->cabeza;
    void* dato = nodo->dato;
    q->cabeza = nodo->siguiente;
    delete nodo;
    q->cant--;
    return dato;
}

// Suelta todos los nodos y los vuelve a enlazar uno por uno: O(n^2).
void l_reordenar(ColaPrioridad* q) {
    NodoLista* resto = q->cabeza;
    q->cabeza = NULL;
    while (resto != NULL) {
        NodoLista* nodo = resto;
        resto = resto->siguiente;
        enlazar(q, nodo);
    }
}

void l_paraCada(ColaPrioridad* q, void (*f)(void*, void*), void* contexto) {
    for (NodoLista* n = q->cabeza; n != NULL; n = n->siguiente) f(n->dato, contexto);
}

void l_liberar(ColaPrioridad* q) {
    while (q->cabeza != NULL) {
        NodoLista* nodo = q->cabeza;
        q->cabeza = nodo->siguiente;
        delete nodo;
    }
}
