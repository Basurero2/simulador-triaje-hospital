#include "ListaOrdenada.hpp"
#include <stdexcept>

ListaOrdenada::ListaOrdenada() : cabeza(nullptr), cantidad(0) {}

ListaOrdenada::~ListaOrdenada() {
    while (!estaVacia()) {
        extraer();
    }
}

void ListaOrdenada::insertar(const Paciente& p) {
    NodoPaciente* nuevo = new NodoPaciente(p);
    if (estaVacia() || p.tieneMayorPrioridadQue(cabeza->dato)){
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
    }else{
        NodoPaciente* actual = cabeza;
        while (actual->siguiente != nullptr && !p.tieneMayorPrioridadQue(actual->siguiente->dato)){
            actual = actual->siguiente;
        }
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }
    cantidad++;
}

Paciente ListaOrdenada::extraer() {
    if (estaVacia()) throw std::runtime_error("Lista vacia");
    NodoPaciente* temp = cabeza;
    Paciente p = temp->dato;
    cabeza = cabeza->siguiente;
    delete temp;
    cantidad--;
    return p;
}

bool ListaOrdenada::estaVacia() const {
    return cabeza == nullptr;
}

int ListaOrdenada::tamano() const {
    return cantidad;
}