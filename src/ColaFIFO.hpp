#ifndef COLAFIFO_HPP
#define COLAFIFO_HPP

#include <cstdlib> 

struct NodoFIFO{
    void* dato;
    NodoFIFO* siguiente;
};

struct ColaFIFO{
    NodoFIFO* frente;
    NodoFIFO* final;
    int cantidad;
    int capacidad;
};

void colaIniciar(ColaFIFO* c, int capacidad){
    c->frente = NULL;
    c->final = NULL;
    c->cantidad = 0;
    c->capacidad = capacidad;
}

int colaVacia(ColaFIFO* c){
    return c->cantidad == 0;
}

int colaLlena(ColaFIFO* c){
    return c->cantidad == c->capacidad;
}

int colaTamano(ColaFIFO* c){
    return c->cantidad;
}

int colaEncolar(ColaFIFO* c, void* dato){
    if(colaLlena(c)) return 0;
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

void *colaDesencolar(ColaFIFO* c){
    if (colaVacia(c)) return NULL;
    NodoFIFO* nodo = c->frente;
    void* dato = nodo->dato;
    c->frente = nodo->siguiente;
    if(c->frente == NULL) c->final = NULL;
    free(nodo);
    c->cantidad--;
    return dato;
}

void colaLiberar(ColaFIFO* c){
    while(c->frente != NULL){
        NodoFIFO* nodo = c->frente;
        c->frente = nodo->siguiente;
        free(nodo);
    }
    c->final = NULL;
    c->cantidad = 0;
}

#endif