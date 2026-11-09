#include "Simulador.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

Simulador::Simulador() : tiempoActual(0), siguienteLlegada(0) {
    reiniciarEstadisticas();
}

void Simulador::reiniciarEstadisticas() {
    totalAtendidos = 0;
    sumaEspera = 0;
    maxEspera = 0;
    tiempoFinal = 0;
    ascensosAging = 0;
    for (int i = 0; i <= NIVELES; i++) {
        sumaEsperaNivel[i] = 0;
        cantidadNivel[i] = 0;
        maxEsperaNivel[i] = 0;
    }
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        ocupado[c] = false;
    }
}

// Ordenamiento por insercion segun tiempoLlegada.
// Es estable: si dos pacientes llegan al mismo tiempo, se respeta el orden del archivo.
void Simulador::ordenarPorLlegada() {
    for (size_t i = 1; i < pacientes.size(); i++) {
        Paciente actual = pacientes[i];
        int j = (int)i - 1;
        while (j >= 0 && pacientes[j].tiempoLlegada > actual.tiempoLlegada) {
            pacientes[j + 1] = pacientes[j];
            j--;
        }
        pacientes[j + 1] = actual;
    }
}

void Simulador::cargarDatosDesdeCSV(const std::string& rutaCSV) {
    pacientes.clear();   // por si se carga mas de una vez

    std::ifstream archivo(rutaCSV);
    if (!archivo.is_open()) {
        std::cout << "[Simulador] ERROR: no se pudo abrir el archivo: " << rutaCSV << "\n";
        return;
    }

    std::string linea;
    int numeroLinea = 0;
    int descartadas = 0;

    while (std::getline(archivo, linea)) {
        numeroLinea++;

        // El archivo puede venir con saltos de linea de Windows (\r\n):
        // quitamos el '\r' sobrante al final de la linea.
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        if (numeroLinea == 1) continue;   // primera linea = encabezado
        if (linea.empty()) continue;      // ignorar lineas en blanco

        // Separar la linea en 4 campos: id,nombre,urgencia,tiempo_llegada
        std::stringstream ss(linea);
        std::string campoId, nombre, campoUrgencia, campoLlegada;
        std::getline(ss, campoId, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, campoUrgencia, ',');
        std::getline(ss, campoLlegada, ',');

        try {
            int id = std::stoi(campoId);
            int urgencia = std::stoi(campoUrgencia);
            int llegada = std::stoi(campoLlegada);

            // Validaciones basicas del dato
            if (nombre.empty() || urgencia < 1 || urgencia > 5 || llegada < 0) {
                throw std::invalid_argument("dato fuera de rango");
            }
            pacientes.push_back(Paciente(id, nombre, urgencia, llegada));
        } catch (const std::exception&) {
            // stoi lanza excepcion si el campo esta vacio o no es un numero
            std::cout << "[Simulador] Linea " << numeroLinea << " invalida, se omite: " << linea << "\n";
            descartadas++;
        }
    }

    ordenarPorLlegada();

    std::cout << "[Simulador] Pacientes cargados: " << pacientes.size()
              << " (lineas omitidas: " << descartadas << ")\n";
}

// Duracion de la consulta: depende solo del id del paciente (entre 5 y 20 min).
// Asi es la misma con heap o con lista, y no hace falta usar numeros aleatorios.
int Simulador::duracionAtencion(const Paciente& p) const {
    return 5 + (p.id * 7) % 16;
}

// El aging cambia la urgencia del paciente dentro de la cola, asi que para las
// estadisticas buscamos su urgencia original en la lista leida del CSV.
int Simulador::urgenciaOriginal(const Paciente& p) const {
    for (size_t i = 0; i < pacientes.size(); i++) {
        if (pacientes[i].id == p.id) {
            return pacientes[i].urgencia;
        }
    }
    return p.urgencia;
}

// Agenda la llegada del proximo paciente del CSV (de a uno, no todos de golpe)
void Simulador::programarSiguienteLlegada() {
    if (siguienteLlegada < pacientes.size()) {
        Evento e;
        e.tiempo = pacientes[siguienteLlegada].tiempoLlegada;
        e.tipo = TipoEvento::LLEGADA_PACIENTE;
        e.paciente = pacientes[siguienteLlegada];
        agenda.insertar(e);
        siguienteLlegada++;
    }
}

// Devuelve el consultorio con espacio en su FIFO y menos carga
// (carga = pacientes esperando + 1 si esta atendiendo). Devuelve -1 si todos estan llenos.
int Simulador::consultorioConMenosCarga() const {
    int mejor = -1;
    int menorCarga = 0;
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        if (filas[c].tamano() < CAPACIDAD_FIFO) {
            int carga = filas[c].tamano() + (ocupado[c] ? 1 : 0);
            if (mejor == -1 || carga < menorCarga) {
                mejor = c;
                menorCarga = carga;
            }
        }
    }
    return mejor;
}

// El consultorio c toma al primero de su FIFO, registra su espera y programa el fin de la consulta
void Simulador::iniciarAtencion(int c) {
    Paciente p = filas[c].desencolar();
    int espera = tiempoActual - p.tiempoLlegada;
    int nivel = urgenciaOriginal(p);

    totalAtendidos++;
    sumaEspera += espera;
    if (espera > maxEspera) maxEspera = espera;
    if (nivel >= 1 && nivel <= NIVELES) {
        sumaEsperaNivel[nivel] += espera;
        cantidadNivel[nivel]++;
        if (espera > maxEsperaNivel[nivel]) maxEsperaNivel[nivel] = espera;
    }

    ocupado[c] = true;
    Evento fin;
    fin.tiempo = tiempoActual + duracionAtencion(p);
    fin.tipo = TipoEvento::FIN_ATENCION;
    fin.paciente = p;
    fin.consultorio = c;
    agenda.insertar(fin);
}

// Mueve pacientes por el sistema hasta que no se pueda avanzar mas:
//   1) consultorios libres empiezan a atender a quien esta en su FIFO
//   2) pacientes de la cola de prioridad pasan a las FIFO que tengan espacio
void Simulador::despachar(ColaPrioridad& colaTriaje) {
    bool huboCambios = true;
    while (huboCambios) {
        huboCambios = false;

        for (int c = 0; c < NUM_CONSULTORIOS; c++) {
            if (!ocupado[c] && !filas[c].estaVacia()) {
                iniciarAtencion(c);
                huboCambios = true;
            }
        }

        while (!colaTriaje.estaVacia()) {
            int c = consultorioConMenosCarga();
            if (c == -1) break;   // todas las FIFO estan llenas
            filas[c].encolar(colaTriaje.extraer());
            huboCambios = true;
        }
    }
}

// Aging: los que llevan mucho esperando en la cola de prioridad suben un nivel (urgencia - 1).
// ColaPrioridad no permite recorrer sus elementos, asi que la sacamos completa,
// modificamos y volvemos a insertar. Funciona igual con heap y con lista.
void Simulador::aplicarAging(ColaPrioridad& colaTriaje) {
    std::vector<Paciente> temporal;
    while (!colaTriaje.estaVacia()) {
        temporal.push_back(colaTriaje.extraer());
    }
    for (size_t i = 0; i < temporal.size(); i++) {
        if (temporal[i].urgencia > 1 && tiempoActual - temporal[i].tiempoLlegada >= UMBRAL_AGING) {
            temporal[i].urgencia--;
            temporal[i].nivelEfectivo = temporal[i].urgencia;
            ascensosAging++;
        }
    }
    for (size_t i = 0; i < temporal.size(); i++) {
        colaTriaje.insertar(temporal[i]);
    }
}

void Simulador::ejecutar(ColaPrioridad& colaTriaje) {
    reiniciarEstadisticas();
    tiempoActual = 0;
    siguienteLlegada = 0;

    if (pacientes.empty()) {
        std::cout << "[Simulador] No hay pacientes cargados. Nada que simular.\n";
        return;
    }

    // Eventos iniciales: la primera llegada y la primera revision de aging
    programarSiguienteLlegada();
    Evento aging;
    aging.tiempo = pacientes[0].tiempoLlegada + INTERVALO_AGING;
    aging.tipo = TipoEvento::AGING_CHECK;
    agenda.insertar(aging);

    // Bucle principal: sacar el evento mas proximo y procesarlo
    while (!agenda.estaVacia()) {
        Evento ev = agenda.sacarSiguiente();
        tiempoActual = ev.tiempo;

        switch (ev.tipo) {
            case TipoEvento::LLEGADA_PACIENTE:
                colaTriaje.insertar(ev.paciente);
                programarSiguienteLlegada();
                despachar(colaTriaje);
                break;

            case TipoEvento::FIN_ATENCION:
                ocupado[ev.consultorio] = false;
                tiempoFinal = tiempoActual;
                despachar(colaTriaje);
                break;

            case TipoEvento::AGING_CHECK:
                aplicarAging(colaTriaje);
                // Solo seguimos revisando si todavia hay pacientes por llegar o esperando
                if (siguienteLlegada < pacientes.size() || !colaTriaje.estaVacia()) {
                    Evento siguiente;
                    siguiente.tiempo = tiempoActual + INTERVALO_AGING;
                    siguiente.tipo = TipoEvento::AGING_CHECK;
                    agenda.insertar(siguiente);
                }
                break;
        }
    }
}

void Simulador::mostrarReporteEstadistico() const {
    if (totalAtendidos == 0) {
        std::cout << "[Simulador] No hay resultados para mostrar.\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n========== REPORTE DE LA SIMULACION ==========\n";
    std::cout << "Consultorios:               " << NUM_CONSULTORIOS << "\n";
    std::cout << "Pacientes atendidos:        " << totalAtendidos << "\n";
    std::cout << "Tiempo total simulado:      " << tiempoFinal << " min\n";
    std::cout << "Espera promedio:            " << (double)sumaEspera / totalAtendidos << " min\n";
    std::cout << "Espera maxima:              " << maxEspera << " min\n";
    std::cout << "Ascensos por aging:         " << ascensosAging << "\n";

    std::cout << "\nEspera por nivel de urgencia (urgencia original):\n";
    std::cout << std::left << std::setw(8) << "Nivel" << std::setw(12) << "Atendidos"
              << std::setw(12) << "Promedio" << "Maxima\n";
    for (int n = 1; n <= NIVELES; n++) {
        std::cout << std::left << std::setw(8) << n << std::setw(12) << cantidadNivel[n];
        if (cantidadNivel[n] > 0) {
            std::cout << std::setw(12) << (double)sumaEsperaNivel[n] / cantidadNivel[n]
                      << maxEsperaNivel[n] << "\n";
        } else {
            std::cout << std::setw(12) << "-" << "-\n";
        }
    }
    std::cout << "==============================================\n";
}
