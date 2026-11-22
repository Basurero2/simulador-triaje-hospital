#include "Paciente.hpp"
#include <cstring>

void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico) {
    p->id = id;
    std::strncpy(p->nombre, nombre, 39);   // maximo 39 caracteres...
    p->nombre[39] = '\0';                  // ...mas el terminador
    p->nivelOriginal = nivel;
    p->nivelEfectivo = nivel;              // al inicio es igual al original
    p->llegada = llegada;
    p->duracion = duracion;
    p->inicio = -1;
    p->fin = -1;
    p->diagnostico = diagnostico;
}

int compararPrioridad(const void* a, const void* b) {
    const Paciente* x = (const Paciente*)a;
    const Paciente* y = (const Paciente*)b;
    if (x->nivelEfectivo != y->nivelEfectivo) return x->nivelEfectivo - y->nivelEfectivo;
    if (x->llegada != y->llegada) return x->llegada - y->llegada;
    return x->id - y->id;                  // desempate final: dos pacientes distintos nunca empatan
}

int compararLlegada(const void* a, const void* b) {
    const Paciente* x = (const Paciente*)a;
    const Paciente* y = (const Paciente*)b;
    if (x->llegada != y->llegada) return x->llegada - y->llegada;
    return x->id - y->id;
}
