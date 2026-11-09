// src/Paciente.cpp - TAD Paciente (implementacion)
#include "Paciente.hpp"
 
void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico) {
    int i = 0;
    while (nombre[i] != '\0' && i < LARGO_NOMBRE - 1) {
        p->nombre[i] = nombre[i];
        i++;
    }
    p->nombre[i] = '\0';
    p->id = id;
    p->nivelOriginal = nivel;
    p->nivelEfectivo = nivel;
    p->llegada = llegada;
    p->duracion = duracion;
    p->inicio = -1;
    p->fin = -1;
    p->diagnostico = diagnostico;
}
 
int compararPrioridad(const void* a, const void* b) {
    const Paciente* x = (const Paciente*) a;
    const Paciente* y = (const Paciente*) b;
    if (x->nivelEfectivo != y->nivelEfectivo) return x->nivelEfectivo - y->nivelEfectivo;
    if (x->llegada != y->llegada) return x->llegada - y->llegada;
    return x->id - y->id;
}
 
int compararLlegada(const void* a, const void* b) {
    const Paciente* x = (const Paciente*) a;
    const Paciente* y = (const Paciente*) b;
    if (x->llegada != y->llegada) return x->llegada - y->llegada;
    return x->id - y->id;
}