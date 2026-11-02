#ifndef BOLSA_HPP
#define BOLSA_HPP

#include "Paciente.hpp"
#include <vector>

// Coleccion no ordenada de elementos
class Bolsa {
private:
    std::vector<Paciente> elementos;

public:
    Bolsa() = default;
    void agregar(const Paciente& p) { elementos.push_back(p); }
    bool estaVacia() const { return elementos.empty(); }
    int tamano() const { return static_cast<int>(elementos.size()); }
    const std::vector<Paciente>& obtenerElementos() const { return elementos; }
};

#endif