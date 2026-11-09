// src/Paciente.hpp - TAD Paciente (declaraciones)
#ifndef PACIENTE_HPP
#define PACIENTE_HPP

#include <string>

#define LARGO_NOMBRE 40

struct Paciente {
    int id;
    char nombre[LARGO_NOMBRE];
    int urgencia;        // 1 (critico / emergencia vital) a 5 (no urgente / consulta general)
    int tiempoLlegada;   // minutos simulados de arribo

    // Campos de compatibilidad con TAD procedimental (Juan Daniel):
    int nivelOriginal;   // 1 (critico) a 5 (no urgente). No cambia nunca
    int nivelEfectivo;   // con este se ordena; el aging puede bajarlo
    int llegada;         // minutos simulados
    int duracion;        // minutos de consulta
    int inicio;          // -1 mientras no empieza
    int fin;             // -1 mientras no termina
    int diagnostico;     // codigo para la Bolsa

    // Constructores para compatibilidad C++ (Bruno / Brithny / Tests)
    Paciente();
    Paciente(int _id, const std::string& _nom = "", int _urg = 5, int _llegada = 0, int _duracion = 0, int _diag = 0);

    // Comparador estricto para desempate FIFO (Estabilidad)
    // Retorna true si 'this' tiene MAYOR prioridad que 'otro'
    bool tieneMayorPrioridadQue(const Paciente& otro) const;
};

// Funciones procedimentales estilo C (TAD Juan Daniel)
// Deja el paciente listo: nivelEfectivo = nivelOriginal, inicio = fin = -1
void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico);

// Negativo si a se atiende antes que b, positivo si despues, 0 si es el mismo.
// Orden: menor nivelEfectivo, luego menor llegada, luego menor id.
int compararPrioridad(const void* a, const void* b);

// Politica sin prioridad: solo por llegada y luego id.
int compararLlegada(const void* a, const void* b);

#endif
