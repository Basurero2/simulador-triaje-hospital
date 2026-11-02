#include "MonticuloBinario.hpp"
#include <stdexcept>

MonticuloBinario::MonticuloBinario() {}

void MonticuloBinario::flotar(int indice) {
    // Implementar heapify-up usando p.tieneMayorPrioridadQue(...)
}

void MonticuloBinario::hundir(int indice) {
    // Implementar heapify-down usando p.tieneMayorPrioridadQue(...)
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