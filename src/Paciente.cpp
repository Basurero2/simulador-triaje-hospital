#include "Paciente.hpp"

Paciente::Paciente(int _id, std::string _nom, int _urg, int _llegada)
    : id(_id), nombre(_nom), urgencia(_urg), tiempoLlegada(_llegada) {}

bool Paciente::tieneMayorPrioridadQue(const Paciente& otro) const {
    if (this->urgencia != otro.urgencia) {
        // En triaje Manchester: 1 es más urgente que 5
        return this->urgencia < otro.urgencia;
    }
    // Si tienen igual urgencia, pasa primero el que llegó antes (FIFO)
    return this->tiempoLlegada < otro.tiempoLlegada;
}