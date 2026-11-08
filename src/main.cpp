#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "MonticuloBinario.hpp"
#include "ListaOrdenada.hpp"
#include "Simulador.hpp"

// Ruta del archivo de pacientes (se ejecuta desde la carpeta raiz del proyecto)
const std::string RUTA_CSV = "data/pacientes_1000.csv";

// Muestra el menu principal
void mostrarMenu() {
    std::cout << "\n=============================================\n";
    std::cout << "  SIMULADOR DE TRIAJE HOSPITALARIO (UNMSM)   \n";
    std::cout << "=============================================\n";
    std::cout << "1. Ejecutar simulacion con Monticulo Binario\n";
    std::cout << "2. Ejecutar simulacion con Lista Ordenada\n";
    std::cout << "3. Comparar rendimiento (Monticulo vs Lista)\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

// Corre una simulacion completa con la cola que le pasemos.
// Gracias al polimorfismo, no importa si es Monticulo o Lista.
void correrSimulacion(ColaPrioridad& cola, const std::string& nombreEstructura) {
    std::cout << "\n--- Simulacion con " << nombreEstructura << " ---\n";

    Simulador sim;  // uno nuevo en cada corrida, para que las estadisticas no se mezclen
    sim.cargarDatosDesdeCSV(RUTA_CSV);
    sim.ejecutar(cola);
    sim.mostrarReporteEstadistico();
}

// ------------------------------------------------------------------
//  OPCION 3: COMPARACION DE RENDIMIENTO
// ------------------------------------------------------------------

// Convierte la diferencia entre dos instantes a milisegundos
double milisegundos(std::chrono::steady_clock::time_point inicio,
                    std::chrono::steady_clock::time_point fin) {
    return std::chrono::duration<double, std::milli>(fin - inicio).count();
}

// Genera n pacientes con urgencia aleatoria (1 a 5) y llegadas crecientes.
// Con srand(42) siempre salen los mismos, asi ambas colas reciben el mismo lote.
std::vector<Paciente> generarLote(int n) {
    std::srand(42);
    std::vector<Paciente> lote;
    for (int i = 1; i <= n; i++) {
        lote.push_back(Paciente(i, "P" + std::to_string(i), 1 + std::rand() % 5, i));
    }
    return lote;
}

// Mide cuanto tarda insertar todo el lote y cuanto tarda extraerlo completo
void medirCola(ColaPrioridad& cola, const std::vector<Paciente>& lote,
               double& msInsertar, double& msExtraer) {
    auto t0 = std::chrono::steady_clock::now();
    for (size_t i = 0; i < lote.size(); i++) {
        cola.insertar(lote[i]);
    }
    auto t1 = std::chrono::steady_clock::now();
    while (!cola.estaVacia()) {
        cola.extraer();
    }
    auto t2 = std::chrono::steady_clock::now();

    msInsertar = milisegundos(t0, t1);
    msExtraer = milisegundos(t1, t2);
}

void compararRendimiento() {
    std::cout << std::fixed << std::setprecision(3);

    // ---- Parte A: la simulacion completa con el CSV, una vez con cada estructura ----
    std::cout << "\n[A] Simulacion completa con " << RUTA_CSV << "\n";
    double msHeap = 0, msLista = 0;
    {
        Simulador sim;
        sim.cargarDatosDesdeCSV(RUTA_CSV);    // la carga del archivo no se mide
        MonticuloBinario heap;
        auto inicio = std::chrono::steady_clock::now();
        sim.ejecutar(heap);
        msHeap = milisegundos(inicio, std::chrono::steady_clock::now());
    }
    {
        Simulador sim;
        sim.cargarDatosDesdeCSV(RUTA_CSV);
        ListaOrdenada lista;
        auto inicio = std::chrono::steady_clock::now();
        sim.ejecutar(lista);
        msLista = milisegundos(inicio, std::chrono::steady_clock::now());
    }
    std::cout << "    Monticulo Binario: " << msHeap << " ms\n";
    std::cout << "    Lista Ordenada:    " << msLista << " ms\n";

    // ---- Parte B: insertar y extraer lotes cada vez mas grandes ----
    std::cout << "\n[B] Insertar y extraer N pacientes (tiempos en ms)\n";
    std::cout << std::left << std::setw(8) << "N"
              << std::setw(16) << "Heap insertar" << std::setw(16) << "Lista insertar"
              << std::setw(16) << "Heap extraer" << "Lista extraer\n";

    const int tamanos[] = {1000, 5000, 10000, 20000};
    for (int k = 0; k < 4; k++) {
        int n = tamanos[k];
        std::vector<Paciente> lote = generarLote(n);

        MonticuloBinario heap;
        ListaOrdenada lista;
        double hIns, hExt, lIns, lExt;
        medirCola(heap, lote, hIns, hExt);
        medirCola(lista, lote, lIns, lExt);

        std::cout << std::left << std::setw(8) << n
                  << std::setw(16) << hIns << std::setw(16) << lIns
                  << std::setw(16) << hExt << lExt << "\n";
    }

    std::cout << "\nLectura: al duplicar N, insertar en la lista deberia tardar ~4 veces\n"
              << "mas (O(n) por insercion, O(n^2) en total), mientras que en el heap\n"
              << "crece apenas un poco mas del doble (O(log n) por insercion).\n";
}

int main() {
    int opcion = -1;

    do {
        mostrarMenu();
        std::cin >> opcion;

        if (opcion == 1) {
            MonticuloBinario heap;
            correrSimulacion(heap, "Monticulo Binario");
        } else if (opcion == 2) {
            ListaOrdenada lista;
            correrSimulacion(lista, "Lista Ordenada");
        } else if (opcion == 3) {
            compararRendimiento();
        } else if (opcion == 0) {
            std::cout << "Saliendo del simulador. Hasta luego.\n";
        } else {
            std::cout << "Opcion no valida. Intente de nuevo.\n";
        }
    }while (opcion != 0) ;

    return 0;
}