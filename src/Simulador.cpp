#include "Simulador.hpp"
#include "Iterador.hpp"
#include "Bolsa.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

void simuladorIniciar(Simulador* s) {
    s->pacientes = NULL;
    s->numPacientes = 0;
    s->siguienteLlegada = 0;
    relojIniciar(&s->reloj, 0);
    s->tipoCola = MONTICULO;
    s->agingActivo = 1;
    s->ascensos = 0;
    s->tiempoFinal = 0;
    s->comparaciones = 0;
    s->ejecutada = 0;
}

void simuladorLiberar(Simulador* s) {
    delete[] s->pacientes;
    s->pacientes = NULL;
    s->numPacientes = 0;
}

// ======================= CARGA DEL CSV =======================

// El CSV no trae duracion ni diagnostico, asi que se calculan a partir del id (siempre igual).
static int duracionDe(int id) { return 5 + (id * 7) % 16; }               // 5 a 20 minutos
static int diagnosticoDe(int id) { return 1 + (id * 5) % NUM_DIAGNOSTICOS; }

int simuladorCargarCSV(Simulador* s, const std::string& ruta) {
    simuladorLiberar(s);

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "[Simulador] ERROR: no se pudo abrir el archivo: " << ruta << "\n";
        return 0;
    }

    std::vector<Paciente> leidos;   // temporal: despues se copia a un arreglo del tamano exacto
    std::string linea;
    int numeroLinea = 0;
    int descartadas = 0;

    while (std::getline(archivo, linea)) {
        numeroLinea++;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();   // saltos de linea de Windows
        if (numeroLinea == 1) continue;                                 // cabecera
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string campoId, nombre, campoNivel, campoLlegada;
        std::getline(ss, campoId, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, campoNivel, ',');
        std::getline(ss, campoLlegada, ',');

        try {
            int id = std::stoi(campoId);
            int nivel = std::stoi(campoNivel);
            int llegada = std::stoi(campoLlegada);
            if (nombre.empty() || nivel < 1 || nivel > NIVELES || llegada < 0) {
                throw std::invalid_argument("dato fuera de rango");
            }
            Paciente p;
            pacienteIniciar(&p, id, nombre.c_str(), nivel, llegada, duracionDe(id), diagnosticoDe(id));
            leidos.push_back(p);
        } catch (const std::exception&) {
            std::cout << "[Simulador] Linea " << numeroLinea << " invalida, se omite: " << linea << "\n";
            descartadas++;
        }
    }

    s->numPacientes = (int)leidos.size();
    s->pacientes = new Paciente[s->numPacientes > 0 ? s->numPacientes : 1];
    for (int i = 0; i < s->numPacientes; i++) s->pacientes[i] = leidos[i];

    // Ordenar por llegada (insercion): la simulacion necesita las llegadas en orden.
    for (int i = 1; i < s->numPacientes; i++) {
        Paciente actual = s->pacientes[i];
        int j = i - 1;
        while (j >= 0 && compararLlegada(&s->pacientes[j], &actual) > 0) {
            s->pacientes[j + 1] = s->pacientes[j];
            j--;
        }
        s->pacientes[j + 1] = actual;
    }

    std::cout << "[Simulador] Pacientes cargados: " << s->numPacientes
              << " (lineas omitidas: " << descartadas << ")\n";
    return s->numPacientes;
}

// ======================= MOTOR DE SIMULACION =======================

// Agenda la llegada del proximo paciente del CSV (de a uno).
static void programarSiguienteLlegada(Simulador* s) {
    if (s->siguienteLlegada < s->numPacientes) {
        Paciente* p = &s->pacientes[s->siguienteLlegada];
        agendaProgramar(&s->agenda, p->llegada, EV_LLEGADA, p, -1);
        s->siguienteLlegada++;
    }
}

// El consultorio c toma al primero de su FIFO y programa el fin de su consulta.
static void iniciarAtencion(Simulador* s, int c) {
    Consultorio* k = &s->consultorios[c];
    Paciente* p = colaDesencolar(&k->fila);
    k->ocupado = 1;
    k->actual = p;
    p->inicio = relojObtener(&s->reloj);
    p->fin = p->inicio + p->duracion;
    agendaProgramar(&s->agenda, p->fin, EV_FIN_ATENCION, p, c);
}

// Consultorio con espacio en su FIFO y menos carga (carga = esperando + 1 si atiende). -1 si todos llenos.
static int consultorioMenorCarga(const Simulador* s) {
    int mejor = -1;
    int menorCarga = 0;
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        const Consultorio* k = &s->consultorios[c];
        if (!colaLlena(&k->fila)) {
            int carga = colaTamano(&k->fila) + k->ocupado;
            if (mejor == -1 || carga < menorCarga) {
                mejor = c;
                menorCarga = carga;
            }
        }
    }
    return mejor;
}

// Pasa pacientes de la cola de prioridad a las FIFO mientras haya espacio.
static void asignarConsultorios(Simulador* s) {
    while (!cpVacia(&s->cola)) {
        int c = consultorioMenorCarga(s);
        if (c == -1) break;
        Paciente* p = (Paciente*)cpExtraer(&s->cola);
        colaEncolar(&s->consultorios[c].fila, p);
        if (!s->consultorios[c].ocupado) iniciarAtencion(s, c);
    }
}

// ---- Aging ----
struct ContextoAging {
    int ahora;
    int ascensos;
};

// Se aplica a cada paciente de la cola: si espero mucho, sube un nivel.
static void envejecer(void* elemento, void* contexto) {
    Paciente* p = (Paciente*)elemento;
    ContextoAging* ctx = (ContextoAging*)contexto;
    if (ctx->ahora - p->llegada >= UMBRAL_AGING && p->nivelEfectivo > 1) {
        p->nivelEfectivo--;
        ctx->ascensos++;
    }
}

static void aplicarAging(Simulador* s) {
    ContextoAging ctx;
    ctx.ahora = relojObtener(&s->reloj);
    ctx.ascensos = 0;
    cpParaCada(&s->cola, envejecer, &ctx);
    if (ctx.ascensos > 0) cpReordenar(&s->cola);   // las prioridades cambiaron: reordenar
    s->ascensos += ctx.ascensos;
}

// ---- Traza paso a paso (para mostrar como funciona) ----
static void imprimirTraza(const Simulador* s, const Evento* ev, int numero) {
    std::cout << "\n[Evento " << numero << "] t=" << ev->tiempo << " min  ";
    if (ev->tipo == EV_LLEGADA) {
        std::cout << "LLEGADA: #" << ev->paciente->id << " " << ev->paciente->nombre
                  << " (nivel " << ev->paciente->nivelOriginal << ")\n";
    } else if (ev->tipo == EV_FIN_ATENCION) {
        std::cout << "FIN_ATENCION: termina #" << ev->paciente->id << " en consultorio "
                  << ev->consultorio + 1 << "\n";
    } else {
        std::cout << "ENVEJECIMIENTO: revision de espera (ascensos hasta ahora: " << s->ascensos << ")\n";
    }
    std::cout << "   Cola de prioridad: " << cpTamano(&s->cola) << " esperando\n";
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        const Consultorio* k = &s->consultorios[c];
        std::cout << "   Consultorio " << c + 1 << ": ";
        if (k->ocupado) std::cout << "atiende #" << k->actual->id << " (hasta t=" << k->actual->fin << ")";
        else std::cout << "libre";
        std::cout << " | fila:";
        if (colaVacia(&k->fila)) std::cout << " -";
        Iterador it;                                  // recorre la FIFO sin sacar a nadie
        iteradorIniciar(&it, &k->fila);
        while (iteradorHaySiguiente(&it)) {
            Paciente* p = iteradorSiguiente(&it);
            std::cout << " #" << p->id << "(N" << p->nivelEfectivo << ")";
        }
        std::cout << "\n";
    }
}

void simuladorEjecutar(Simulador* s, TipoCola tipo, int agingActivo, int trazaEventos) {
    if (s->numPacientes == 0) {
        std::cout << "[Simulador] No hay pacientes cargados. Nada que simular.\n";
        return;
    }

    // Preparar: la misma carga de pacientes se puede simular varias veces
    for (int i = 0; i < s->numPacientes; i++) {
        Paciente* p = &s->pacientes[i];
        p->nivelEfectivo = p->nivelOriginal;
        p->inicio = -1;
        p->fin = -1;
    }
    s->tipoCola = tipo;
    s->agingActivo = agingActivo;
    s->ascensos = 0;
    s->tiempoFinal = 0;
    s->siguienteLlegada = 0;
    relojIniciar(&s->reloj, 0);
    agendaIniciar(&s->agenda);
    cpIniciar(&s->cola, tipo, compararPrioridad);
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        colaIniciar(&s->consultorios[c].fila, CAPACIDAD_FIFO);
        s->consultorios[c].ocupado = 0;
        s->consultorios[c].actual = NULL;
        s->consultorios[c].atendidos = 0;
        s->consultorios[c].minutosOcupado = 0;
    }

    // Eventos iniciales: la primera llegada y (si esta activo) la primera revision de aging
    programarSiguienteLlegada(s);
    if (agingActivo) {
        agendaProgramar(&s->agenda, s->pacientes[0].llegada + INTERVALO_AGING, EV_ENVEJECIMIENTO, NULL, -1);
    }

    // Bucle principal: sacar el evento mas proximo, avanzar el reloj y procesarlo
    int procesados = 0;
    while (!agendaVacia(&s->agenda)) {
        Evento* ev = agendaExtraerSiguiente(&s->agenda);
        relojAvanzar(&s->reloj, ev->tiempo);

        if (ev->tipo == EV_LLEGADA) {
            cpInsertar(&s->cola, ev->paciente);
            programarSiguienteLlegada(s);
            asignarConsultorios(s);
        } else if (ev->tipo == EV_FIN_ATENCION) {
            Consultorio* k = &s->consultorios[ev->consultorio];
            k->ocupado = 0;
            k->actual = NULL;
            k->atendidos++;
            k->minutosOcupado += ev->paciente->duracion;
            if (ev->paciente->fin > s->tiempoFinal) s->tiempoFinal = ev->paciente->fin;
            if (!colaVacia(&k->fila)) iniciarAtencion(s, ev->consultorio);
            asignarConsultorios(s);
        } else {   // EV_ENVEJECIMIENTO
            aplicarAging(s);
            // Se vuelve a programar solo si todavia queda trabajo pendiente
            if (s->siguienteLlegada < s->numPacientes || !cpVacia(&s->cola)) {
                agendaProgramar(&s->agenda, relojObtener(&s->reloj) + INTERVALO_AGING, EV_ENVEJECIMIENTO, NULL, -1);
            }
        }

        procesados++;
        if (trazaEventos > 0 && procesados <= trazaEventos) imprimirTraza(s, ev, procesados);
        delete ev;   // el evento lo reservo agendaProgramar; lo liberamos al procesarlo
    }

    s->comparaciones = s->cola.comparaciones;
    cpLiberar(&s->cola);
    agendaLiberar(&s->agenda);
    for (int c = 0; c < NUM_CONSULTORIOS; c++) colaLiberar(&s->consultorios[c].fila);
    s->ejecutada = 1;
}

// ======================= REPORTE =======================

void simuladorReporte(const Simulador* s) {
    if (!s->ejecutada) {
        std::cout << "[Simulador] No hay resultados para mostrar.\n";
        return;
    }

    // Las metricas se calculan recorriendo los pacientes: espera = inicio - llegada
    int atendidos = 0, maxEspera = 0;
    long sumaEspera = 0;
    long sumaNivel[NIVELES + 1] = {0};
    int cantNivel[NIVELES + 1] = {0};
    int maxNivel[NIVELES + 1] = {0};
    Bolsa porDiagnostico;
    bolsaIniciar(&porDiagnostico);

    for (int i = 0; i < s->numPacientes; i++) {
        const Paciente* p = &s->pacientes[i];
        if (p->inicio < 0) continue;
        int espera = p->inicio - p->llegada;
        atendidos++;
        sumaEspera += espera;
        if (espera > maxEspera) maxEspera = espera;
        int n = p->nivelOriginal;   // por nivel ORIGINAL (el aging cambia el efectivo)
        sumaNivel[n] += espera;
        cantNivel[n]++;
        if (espera > maxNivel[n]) maxNivel[n] = espera;
        bolsaAgregar(&porDiagnostico, p->diagnostico);
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n========== REPORTE DE LA SIMULACION ==========\n";
    std::cout << "Cola de prioridad:          " << (s->tipoCola == MONTICULO ? "Monticulo binario" : "Lista ordenada") << "\n";
    std::cout << "Aging:                      " << (s->agingActivo ? "activado" : "desactivado") << "\n";
    std::cout << "Pacientes atendidos:        " << atendidos << "\n";
    std::cout << "Tiempo total simulado:      " << s->tiempoFinal << " min\n";
    std::cout << "Espera promedio:            " << (atendidos ? (double)sumaEspera / atendidos : 0.0) << " min\n";
    std::cout << "Espera maxima:              " << maxEspera << " min\n";
    std::cout << "Ascensos por aging:         " << s->ascensos << "\n";
    std::cout << "Comparaciones de la cola:   " << s->comparaciones << "\n";

    std::cout << "\nEspera por nivel de urgencia (nivel original):\n";
    std::cout << std::left << std::setw(8) << "Nivel" << std::setw(12) << "Atendidos"
              << std::setw(12) << "Promedio" << "Maxima\n";
    for (int n = 1; n <= NIVELES; n++) {
        std::cout << std::left << std::setw(8) << n << std::setw(12) << cantNivel[n];
        if (cantNivel[n] > 0) {
            std::cout << std::setw(12) << (double)sumaNivel[n] / cantNivel[n] << maxNivel[n] << "\n";
        } else {
            std::cout << std::setw(12) << "-" << "-\n";
        }
    }

    std::cout << "\nOcupacion de los consultorios:\n";
    std::cout << std::left << std::setw(14) << "Consultorio" << std::setw(12) << "Atendidos" << "Ocupacion\n";
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        double ocup = s->tiempoFinal > 0 ? 100.0 * s->consultorios[c].minutosOcupado / s->tiempoFinal : 0.0;
        std::cout << std::left << std::setw(14) << c + 1 << std::setw(12) << s->consultorios[c].atendidos
                  << ocup << " %\n";
    }

    std::cout << "\nPacientes atendidos por diagnostico:\n";
    for (int d = 1; d <= NUM_DIAGNOSTICOS; d++) {
        std::cout << "  Diagnostico " << d << ": " << bolsaContar(&porDiagnostico, d) << "\n";
    }
    bolsaLiberar(&porDiagnostico);
    std::cout << "==============================================\n";
}
