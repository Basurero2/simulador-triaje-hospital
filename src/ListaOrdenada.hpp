#ifndef LISTA_ORDENADA_HPP
#define LISTA_ORDENADA_HPP

#include "ColaPrioridad.hpp"

struct NodoPaciente {
    Paciente dato;
    NodoPaciente* siguiente;
    NodoPaciente(const Paciente& p) : dato(p), siguiente(nullptr) {}
};

class ListaOrdenada : public ColaPrioridad {
private:
    NodoPaciente* cabeza;
    int cantidad;

public:
    ListaOrdenada();
    ~ListaOrdenada() override;

    void insertar(const Paciente& p) override;
    Paciente extraer() override;
    bool estaVacia() const override;
    int tamano() const override;
};

#endif