#ifndef PACIENTE_HPP
#define PACIENTE_HPP

// TAD Paciente (Tabla 5 del informe). Estilo procedimental: struct + funciones.
struct Paciente {
    int id;
    char nombre[40];
    int nivelOriginal;   // nivel de triaje (1 critico ... 5 no urgente); nunca cambia
    int nivelEfectivo;   // nivel con el que se ordena la atencion; solo el aging lo baja
    int llegada;         // minuto en que llega al triaje
    int duracion;        // minutos que dura su consulta
    int inicio;          // minuto en que empieza su atencion (-1 si aun no)
    int fin;             // minuto en que termina su atencion (-1 si aun no)
    int diagnostico;     // codigo de diagnostico (lo usa la Bolsa)
};

void pacienteIniciar(Paciente* p, int id, const char* nombre, int nivel,
                     int llegada, int duracion, int diagnostico);

// Devuelven negativo si 'a' se atiende antes que 'b', positivo si despues.
// Reciben void* para poder usarse como funcion de comparacion de la cola de prioridad.
int compararPrioridad(const void* a, const void* b);  // nivelEfectivo, luego llegada, luego id
int compararLlegada(const void* a, const void* b);    // llegada, luego id

#endif
