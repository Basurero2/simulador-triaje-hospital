#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>

#include "../src/Paciente.hpp"
#include "../src/ColaPrioridad.hpp"
#include "../src/MonticuloBinario.hpp"
#include "../src/ListaOrdenada.hpp"

std::vector<Paciente> generarLotePrueba(int cantidad) {
    std::vector<Paciente> lote;
    lote.reserve(cantidad);
    srand(42); // Semilla fija reproducible

    for (int i = 1; i <= cantidad; ++i) {
        std::string nombre = "Paciente_" + std::to_string(i);
        int urgencia = (rand() % 5) + 1; // Prioridad del 1 al 5
        int tiempoLlegada = i * 2;       // Tiempo de arribo creciente
        lote.emplace_back(i, nombre, urgencia, tiempoLlegada);
    }
    return lote;
}

void testEquivalenciaHeapVsLista() {
    std::cout << "====================================================\n";
    std::cout << "  INICIANDO PLAN DE PRUEBAS: EQUIVALENCIA DE COLAS  \n";
    std::cout << "====================================================\n\n";

    MonticuloBinario colaHeap;
    ListaOrdenada colaLista;

    const int TOTAL_PACIENTES = 100;
    std::vector<Paciente> pacientesPrueba = generarLotePrueba(TOTAL_PACIENTES);

    std::cout << "[1] Insertando " << TOTAL_PACIENTES << " pacientes en ambas estructuras...\n";
    for (const auto& p : pacientesPrueba) {
        colaHeap.insertar(p);
        colaLista.insertar(p);
    }

    assert(colaHeap.tamano() == TOTAL_PACIENTES);
    assert(colaLista.tamano() == TOTAL_PACIENTES);
    std::cout << "[2] Carga completada exitosamente.\n\n";

    std::cout << "[3] Extrayendo y verificando orden exacto de atencion...\n";
    int atencion = 1;
    bool discrepancia = false;

    while (!colaHeap.estaVacia() && !colaLista.estaVacia()) {
        Paciente pHeap = colaHeap.extraer();
        Paciente pLista = colaLista.extraer();

        if (pHeap.id != pLista.id || pHeap.urgencia != pLista.urgencia) {
            std::cerr << " -> ERROR en turno #" << atencion << ":\n"
                      << "    Heap  : ID " << pHeap.id  << " (Urg " << pHeap.urgencia  << ", T=" << pHeap.tiempoLlegada << ")\n"
                      << "    Lista : ID " << pLista.id << " (Urg " << pLista.urgencia << ", T=" << pLista.tiempoLlegada << ")\n";
            discrepancia = true;
            assert(false);
        }
        atencion++;
    }

    assert(colaHeap.estaVacia() && colaLista.estaVacia());

    if (!discrepancia) {
        std::cout << "\n>>> RESULTADO: TODAS LAS PRUEBAS PASARON EXITOSAMENTE (100/100).\n";
        std::cout << ">>> Ambas estructuras son funcionalmente equivalentes.\n";
    }
}

int main() {
    testEquivalenciaHeapVsLista();
    return 0;
}