#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <string>
#include "Simulador.hpp"

// Ruta del archivo de pacientes (se ejecuta desde la carpeta raiz del proyecto)
const std::string RUTA_CSV = "data/pacientes_1000.csv";

void mostrarMenu(int agingActivo) {
    std::cout << "\n=============================================\n";
    std::cout << "  SIMULADOR DE TRIAJE HOSPITALARIO (UNMSM)   \n";
    std::cout << "=============================================\n";
    std::cout << "1. Simulacion con Monticulo Binario\n";
    std::cout << "2. Simulacion con Lista Ordenada\n";
    std::cout << "3. Comparar rendimiento (Monticulo vs Lista)\n";
    std::cout << "4. Ver traza paso a paso (primeros 12 eventos)\n";
    std::cout << "5. Aging: " << (agingActivo ? "ACTIVADO" : "DESACTIVADO") << " (cambiar)\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

double milisegundos(std::chrono::steady_clock::time_point inicio,
                    std::chrono::steady_clock::time_point fin) {
    return std::chrono::duration<double, std::milli>(fin - inicio).count();
}

// Mide cuanto tarda insertar todo el lote y extraerlo completo, y cuantas comparaciones hizo.
void medirCola(TipoCola tipo, Paciente* lote, int n,
               double* msInsertar, double* msExtraer, long* comparaciones) {
    ColaPrioridad q;
    cpIniciar(&q, tipo, compararPrioridad);

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) cpInsertar(&q, &lote[i]);
    auto t1 = std::chrono::steady_clock::now();
    while (!cpVacia(&q)) cpExtraer(&q);
    auto t2 = std::chrono::steady_clock::now();

    *msInsertar = milisegundos(t0, t1);
    *msExtraer = milisegundos(t1, t2);
    *comparaciones = q.comparaciones;
    cpLiberar(&q);
}

void compararRendimiento(Simulador* sim, int agingActivo) {
    std::cout << std::fixed << std::setprecision(3);

    // ---- A: la simulacion completa con el CSV, una vez con cada estructura ----
    std::cout << "\n[A] Simulacion completa con " << RUTA_CSV << "\n";
    if (sim->numPacientes == 0) {
        std::cout << "    (no hay pacientes cargados)\n";
    } else {
        auto t0 = std::chrono::steady_clock::now();
        simuladorEjecutar(sim, MONTICULO, agingActivo, 0);
        double msHeap = milisegundos(t0, std::chrono::steady_clock::now());
        long compHeap = sim->comparaciones;

        t0 = std::chrono::steady_clock::now();
        simuladorEjecutar(sim, LISTA, agingActivo, 0);
        double msLista = milisegundos(t0, std::chrono::steady_clock::now());
        long compLista = sim->comparaciones;

        std::cout << "    Monticulo Binario: " << msHeap << " ms, " << compHeap << " comparaciones\n";
        std::cout << "    Lista Ordenada:    " << msLista << " ms, " << compLista << " comparaciones\n";
    }

    // ---- B: insertar y extraer lotes cada vez mas grandes ----
    std::cout << "\n[B] Insertar y extraer N pacientes (tiempos en ms)\n";
    std::cout << std::left << std::setw(8) << "N"
              << std::setw(15) << "Heap insertar" << std::setw(15) << "Lista insertar"
              << std::setw(15) << "Heap extraer" << std::setw(15) << "Lista extraer"
              << std::setw(14) << "Comp. heap" << "Comp. lista\n";

    const int tamanos[] = {1000, 5000, 10000, 20000};
    for (int k = 0; k < 4; k++) {
        int n = tamanos[k];
        Paciente* lote = new Paciente[n];
        std::srand(42);   // mismo lote siempre
        for (int i = 0; i < n; i++) {
            pacienteIniciar(&lote[i], i + 1, "P", 1 + std::rand() % 5, i + 1, 10, 1);
        }

        double hIns, hExt, lIns, lExt;
        long hComp, lComp;
        medirCola(MONTICULO, lote, n, &hIns, &hExt, &hComp);
        medirCola(LISTA, lote, n, &lIns, &lExt, &lComp);

        std::cout << std::left << std::setw(8) << n
                  << std::setw(15) << hIns << std::setw(15) << lIns
                  << std::setw(15) << hExt << std::setw(15) << lExt
                  << std::setw(14) << hComp << lComp << "\n";
        delete[] lote;
    }

    std::cout << "\nLectura: al duplicar N, insertar en la lista tarda ~4 veces mas\n"
              << "(O(n) por insercion, O(n^2) en total), mientras que en el monticulo\n"
              << "crece apenas un poco mas del doble (O(log n) por insercion).\n";
}

int main() {
    Simulador sim;
    simuladorIniciar(&sim);
    simuladorCargarCSV(&sim, RUTA_CSV);

    int agingActivo = 1;
    int opcion;

    do {
        mostrarMenu(agingActivo);
        std::cin >> opcion;

        if (opcion == 1) {
            simuladorEjecutar(&sim, MONTICULO, agingActivo, 0);
            simuladorReporte(&sim);
        } else if (opcion == 2) {
            simuladorEjecutar(&sim, LISTA, agingActivo, 0);
            simuladorReporte(&sim);
        } else if (opcion == 3) {
            compararRendimiento(&sim, agingActivo);
        } else if (opcion == 4) {
            simuladorEjecutar(&sim, MONTICULO, agingActivo, 12);
        } else if (opcion == 5) {
            agingActivo = !agingActivo;
            std::cout << "Aging " << (agingActivo ? "activado" : "desactivado") << ".\n";
        } else if (opcion == 0) {
            std::cout << "Saliendo del simulador. Hasta luego.\n";
        } else {
            std::cout << "Opcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 0);

    simuladorLiberar(&sim);
    return 0;
}
