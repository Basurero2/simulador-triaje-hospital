#ifndef AGENDA_EVENTOS_HPP
#define AGENDA_EVENTOS_HPP

#include "Paciente.hpp"
#include <string>

enum class TipoEvento { LLEGADA_PACIENTE, FIN_ATENCION, AGING_CHECK };

struct Evento {
    int tiempo;
    TipoEvento tipo;
    Paciente paciente;

    bool operator>(const Evento& otro) const {
        return this->tiempo > otro.tiempo;
    }
};

class AgendaEventos {
    // Prototipo de gestion de eventos discretos
};

#endif