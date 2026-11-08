#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

#include <string>
#include <vector>
#include "ColaPrioridad.hpp"
#include "ColaFIFO.hpp"
#include "AgendaEventos.hpp"

class Simulador {
private:
    // ---- Parametros de la simulacion (se pueden ajustar aqui) ----
    static constexpr int NUM_CONSULTORIOS = 3;
    static constexpr int CAPACIDAD_FIFO   = 2;   // pacientes que pueden esperar en la FIFO de cada consultorio
    static constexpr int INTERVALO_AGING  = 30;  // cada cuantos minutos se revisa el aging
    static constexpr int UMBRAL_AGING     = 60;  // minutos de espera para subir un nivel de urgencia
    static constexpr int NIVELES          = 5;

    // ---- Estado de la simulacion ----
    int tiempoActual;
    std::vector<Paciente> pacientes;   // pacientes del CSV, ordenados por llegada (no se modifican)
    size_t siguienteLlegada;           // indice del proximo paciente que va a llegar
    AgendaEventos agenda;
    ColaFIFO filas[NUM_CONSULTORIOS];  // una cola FIFO por consultorio
    bool ocupado[NUM_CONSULTORIOS];    // true si el consultorio esta atendiendo a alguien

    // ---- Estadisticas (los niveles se cuentan con la urgencia ORIGINAL del paciente) ----
    int totalAtendidos;
    long sumaEspera;
    int maxEspera;
    int tiempoFinal;
    int ascensosAging;
    long sumaEsperaNivel[NIVELES + 1];   // se usan las posiciones 1..5
    int cantidadNivel[NIVELES + 1];
    int maxEsperaNivel[NIVELES + 1];

    // ---- Metodos auxiliares ----
    void ordenarPorLlegada();
    void reiniciarEstadisticas();
    int duracionAtencion(const Paciente& p) const;
    int urgenciaOriginal(const Paciente& p) const;
    void programarSiguienteLlegada();
    int consultorioConMenosCarga() const;
    void iniciarAtencion(int c);
    void despachar(ColaPrioridad& colaTriaje);
    void aplicarAging(ColaPrioridad& colaTriaje);

public:
    Simulador();
    void cargarDatosDesdeCSV(const std::string& rutaCSV);
    void ejecutar(ColaPrioridad& colaTriaje);
    void mostrarReporteEstadistico() const;
};

#endif