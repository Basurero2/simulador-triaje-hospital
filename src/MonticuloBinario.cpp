#include "MonticuloBinario.hpp"
#include <stdexcept>
#include <utility>

MonticuloBinario::MonticuloBinario() {}

void MonticuloBinario::flotar(int indice) {
    while(indice > 0) {
        int padre = (indice - 1) / 2;
        if (heap[indice].tieneMayorPrioridadQue(heap[padre])) {
            std::swap(heap[indice], heap[padre]);
            indice = padre;
        } else {
            break;
        }
    }
}

void MonticuloBinario::hundir(int indice) {
    int n = heap.size();
    while (true){
        int hijoIzq = 2 * indice + 1;
        int hijoDer = 2 * indice + 2;
        int mayor = indice;

        if (hijoIzq < n && heap[hijoIzq].tieneMayorPrioridadQue(heap[mayor])) {
            mayor = hijoIzq;
        }
        if (hijoDer < n && heap[hijoDer].tieneMayorPrioridadQue(heap[mayor])) {
            mayor = hijoDer;
        }
        if (mayor != indice) {
            std::swap(heap[indice], heap[mayor]);
            indice = mayor;
        } else {
            break;
        }
    }
}

void MonticuloBinario::insertar(const Paciente& p) {
    heap.push_back(p);
    flotar(heap.size() - 1);
}

Paciente MonticuloBinario::extraer() {
    if (estaVacia()) throw std::runtime_error("Heap vacio");
    Paciente raiz = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!estaVacia()) hundir(0);
    return raiz;
}

bool MonticuloBinario::estaVacia() const {
    return heap.empty();
}

int MonticuloBinario::tamano() const {
    return static_cast<int>(heap.size());
}