#include "ListaOrdenada.hpp"
#include <stdexcept>

ListaOrdenada::ListaOrdenada() : cabeza(nullptr), cantidad(0) {}

ListaOrdenada::~ListaOrdenada() {
    while (!estaVacia()) {
        extraer();
    }
}

void ListaOrdenada::insertar(const Paciente& p) {
    // Implementar insercion secuencial O(n) usando p.tieneMayorPrioridadQue(...)
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