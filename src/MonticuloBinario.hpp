#ifndef MONTICULO_BINARIO_HPP
#define MONTICULO_BINARIO_HPP

#include "ColaPrioridad.hpp"
#include <vector>

class MonticuloBinario : public ColaPrioridad {
private:
    std::vector<Paciente> heap;

    void flotar(int indice);
    void hundir(int indice);

public:
    MonticuloBinario();
    ~MonticuloBinario() override = default;

    void insertar(const Paciente& p) override;
    Paciente extraer() override;
    bool estaVacia() const override;
    int tamano() const override;
};

#endif