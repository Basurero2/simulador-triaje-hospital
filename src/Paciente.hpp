// src/Paciente.hpp - TAD Paciente (declaraciones)
#ifndef PACIENTE_HPP
#define PACIENTE_HPP

#define LARGO_NOMBRE 40

struct Paciente {
    int id;
    char nombre[LARGO_NOMBRE];
    int nivelOriginal;   // 1 (critico) a 5 (no urgente). No cambia nunca
    int nivelEfectivo;   // con este se ordena; el aging puede bajarlo
    int llegada;         // minutos simulados
    int duracion;        // minutos de consulta
    int inicio;          // -1 mientras no empieza
    int fin;             // -1 mientras no termina
    int diagnostico;     // codigo para la Bolsa
};
 
// Deja el paciente listo: nivelEfectivo = nivelOriginal, inicio = fin = -1
void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico);
 
// Negativo si a se atiende antes que b, positivo si despues, 0 si es el mismo.
// Orden: menor nivelEfectivo, luego menor llegada, luego menor id.
int compararPrioridad(const void* a, const void* b);
 
// Politica sin prioridad: solo por llegada y luego id.
int compararLlegada(const void* a, const void* b);



#endif