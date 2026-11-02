#ifndef PACIENTE_HPP
#define PACIENTE_HPP

#include <string>
#include <iostream>

struct Paciente {
    int id;
    std::string nombre;
    int urgencia;       // 1 (Emergencia vital) a 5 (Consulta general)
    int tiempoLlegada;  // Minuto o segundo de llegada al triaje

    Paciente(int _id = 0, std::string _nom = "", int _urg = 5, int _llegada = 0);

    // Comparador estricto para desempate FIFO (Estabilidad)
    // Retorna true si 'this' tiene MAYOR prioridad que 'otro'
    bool tieneMayorPrioridadQue(const Paciente& otro) const;
};

#endif