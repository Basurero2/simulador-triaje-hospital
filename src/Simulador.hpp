#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

#include <string>
#include "Paciente.hpp"
#include "ColaFIFO.hpp"
#include "ColaPrioridad.hpp"
#include "AgendaEventos.hpp"

// ---- Parametros de la simulacion (supuestos de la seccion 3.3 del informe) ----
const int NUM_CONSULTORIOS = 3;
const int CAPACIDAD_FIFO   = 2;    // pacientes que pueden esperar en la FIFO de cada consultorio
const int INTERVALO_AGING  = 30;   // cada cuantos minutos se revisa el aging
const int UMBRAL_AGING     = 60;   // minutos de espera para subir un nivel
const int NIVELES          = 5;
const int NUM_DIAGNOSTICOS = 6;

// TAD Reloj de simulacion (Tabla 3). Nunca retrocede.
struct Reloj {
    int tactual;
};
inline void relojIniciar(Reloj* r, int tinicio) { r->tactual = tinicio; }
inline int relojObtener(const Reloj* r) { return r->tactual; }
inline int relojAvanzar(Reloj* r, int tnuevo) {
    if (tnuevo < r->tactual) return 0;   // no se puede retroceder
    r->tactual = tnuevo;
    return 1;
}

struct Consultorio {
    ColaFIFO fila;          // pacientes esperando en este consultorio
    int ocupado;            // 1 si esta atendiendo a alguien
    Paciente* actual;       // paciente en consulta (NULL si esta libre)
    int atendidos;
    int minutosOcupado;
};

struct Simulador {
    Paciente* pacientes;    // arreglo propio del simulador (se reserva una vez y se libera al final)
    int numPacientes;
    int siguienteLlegada;   // indice del proximo paciente que va a llegar
    Reloj reloj;
    Agenda agenda;
    ColaPrioridad cola;     // cola de prioridad general (triaje)
    Consultorio consultorios[NUM_CONSULTORIOS];

    // configuracion y resultados de la ultima ejecucion
    TipoCola tipoCola;
    int agingActivo;
    int ascensos;           // cuantas veces el aging subio un nivel
    int tiempoFinal;
    long comparaciones;     // comparaciones hechas por la cola de prioridad
    int ejecutada;
};

void simuladorIniciar(Simulador* s);
int  simuladorCargarCSV(Simulador* s, const std::string& ruta);   // devuelve cuantos pacientes cargo
void simuladorEjecutar(Simulador* s, TipoCola tipo, int agingActivo, int trazaEventos);
// Para ejecutar paso a paso (los usa la interfaz gráfica). simuladorEjecutar sigue funcionando igual.
int  simuladorPreparar(Simulador* s, TipoCola tipo, int agingActivo);   
int  simuladorPaso(Simulador* s, Evento* procesado);                   
void simuladorFinalizar(Simulador* s);                                 
void simuladorReporte(const Simulador* s);
void simuladorLiberar(Simulador* s);

#endif
