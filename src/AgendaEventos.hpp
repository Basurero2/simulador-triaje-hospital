#ifndef AGENDA_EVENTOS_HPP
#define AGENDA_EVENTOS_HPP

#include <cstddef>
#include "Paciente.hpp"
#include "ColaPrioridad.hpp"

// Agenda de eventos (FEL, Tabla 4): un montículo (ColaPrioridad) de eventos ordenados por tiempo.
// Si dos eventos coinciden en el tiempo, el orden del enum decide: primero terminan las
// consultas (se libera el consultorio), luego llegan pacientes y al final se revisa el aging.
enum TipoEvento { EV_FIN_ATENCION = 0, EV_LLEGADA = 1, EV_ENVEJECIMIENTO = 2 };

struct Evento {
    int tiempo;
    int tipo;
    Paciente* paciente;   // NULL en ENVEJECIMIENTO
    int consultorio;      // solo se usa en FIN_ATENCION
    long orden;           // orden de programacion: desempata eventos iguales
};

inline int compararEvento(const void* a, const void* b) {
    const Evento* x = (const Evento*)a;
    const Evento* y = (const Evento*)b;
    if (x->tiempo != y->tiempo) return x->tiempo - y->tiempo;
    if (x->tipo != y->tipo) return x->tipo - y->tipo;
    if (x->orden < y->orden) return -1;
    if (x->orden > y->orden) return 1;
    return 0;
}

struct Agenda {
    ColaPrioridad cola;
    long contador;        // cuantos eventos se han programado (para 'orden')
};

inline void agendaIniciar(Agenda* a) {
    cpIniciar(&a->cola, MONTICULO, compararEvento);
    a->contador = 0;
}

// Reserva el evento con new. Quien lo extrae debe liberarlo con delete.
inline void agendaProgramar(Agenda* a, int tiempo, int tipo, Paciente* p, int consultorio) {
    Evento* e = new Evento;
    e->tiempo = tiempo;
    e->tipo = tipo;
    e->paciente = p;
    e->consultorio = consultorio;
    e->orden = a->contador++;
    cpInsertar(&a->cola, e);
}

inline Evento* agendaExtraerSiguiente(Agenda* a) { return (Evento*)cpExtraer(&a->cola); }
inline Evento* agendaVerSiguiente(const Agenda* a) { return (Evento*)cpVer(&a->cola); }
inline int agendaVacia(const Agenda* a) { return cpVacia(&a->cola); }
inline int agendaTamano(const Agenda* a) { return cpTamano(&a->cola); }

inline void liberarEventoCb(void* e, void*) { delete (Evento*)e; }

// Libera los eventos que queden y la estructura.
inline void agendaLiberar(Agenda* a) {
    cpParaCada(&a->cola, liberarEventoCb, NULL);
    cpLiberar(&a->cola);
}

#endif
