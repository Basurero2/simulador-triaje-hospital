// src/Paciente.cpp - TAD Paciente (implementacion)
#include "Paciente.hpp"

Paciente::Paciente()
    : id(0), urgencia(5), tiempoLlegada(0),
      nivelOriginal(5), nivelEfectivo(5), llegada(0),
      duracion(0), inicio(-1), fin(-1), diagnostico(0) {
    nombre[0] = '\0';
}

Paciente::Paciente(int _id, const std::string& _nom, int _urg, int _llegada, int _duracion, int _diag)
    : id(_id), urgencia(_urg), tiempoLlegada(_llegada),
      nivelOriginal(_urg), nivelEfectivo(_urg), llegada(_llegada),
      duracion(_duracion), inicio(-1), fin(-1), diagnostico(_diag) {
    int i = 0;
    while (i < (int)_nom.size() && i < LARGO_NOMBRE - 1) {
        nombre[i] = _nom[i];
        i++;
    }
    nombre[i] = '\0';
}

bool Paciente::tieneMayorPrioridadQue(const Paciente& otro) const {
    // Sincronizar por si se accedio directamente a urgencia o nivelEfectivo
    int n1 = (this->nivelEfectivo != 0) ? this->nivelEfectivo : this->urgencia;
    int n2 = (otro.nivelEfectivo != 0) ? otro.nivelEfectivo : otro.urgencia;

    if (n1 != n2) {
        // En triaje Manchester: 1 es mas urgente que 5
        return n1 < n2;
    }

    int t1 = (this->tiempoLlegada != 0) ? this->tiempoLlegada : this->llegada;
    int t2 = (otro.tiempoLlegada != 0) ? otro.tiempoLlegada : otro.llegada;

    if (t1 != t2) {
        return t1 < t2;
    }

    return this->id < otro.id;
}

void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico) {
    int i = 0;
    while (nombre[i] != '\0' && i < LARGO_NOMBRE - 1) {
        p->nombre[i] = nombre[i];
        i++;
    }
    p->nombre[i] = '\0';
    p->id = id;
    p->urgencia = nivel;
    p->tiempoLlegada = llegada;
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
    int nx = (x->nivelEfectivo != 0) ? x->nivelEfectivo : x->urgencia;
    int ny = (y->nivelEfectivo != 0) ? y->nivelEfectivo : y->urgencia;
    if (nx != ny) return nx - ny;

    int tx = (x->llegada != 0) ? x->llegada : x->tiempoLlegada;
    int ty = (y->llegada != 0) ? y->llegada : y->tiempoLlegada;
    if (tx != ty) return tx - ty;

    return x->id - y->id;
}

int compararLlegada(const void* a, const void* b) {
    const Paciente* x = (const Paciente*) a;
    const Paciente* y = (const Paciente*) b;
    int tx = (x->llegada != 0) ? x->llegada : x->tiempoLlegada;
    int ty = (y->llegada != 0) ? y->llegada : y->tiempoLlegada;
    if (tx != ty) return tx - ty;

    return x->id - y->id;
}
