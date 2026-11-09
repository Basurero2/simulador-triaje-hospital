#ifndef AGENDA_EVENTOS_HPP
#define AGENDA_EVENTOS_HPP

#include "Paciente.hpp"
#include <string>

enum class TipoEvento { LLEGADA_PACIENTE, FIN_ATENCION, AGING_CHECK };

struct Evento {
    int tiempo = 0;
    TipoEvento tipo = TipoEvento::LLEGADA_PACIENTE;
    Paciente paciente;
    int consultorio = -1;   // solo se usa en FIN_ATENCION: que consultorio termino

    bool operator>(const Evento& otro) const {
        return this->tiempo > otro.tiempo;
    }
};

// Nodo de la lista enlazada de eventos
struct NodoEvento {
    Evento dato;
    NodoEvento* siguiente;
    NodoEvento(const Evento& e) : dato(e), siguiente(nullptr) {}
};

// Agenda de eventos: lista enlazada ORDENADA por tiempo.
// El evento mas cercano en el tiempo siempre esta al frente.
// Si dos eventos tienen el mismo tiempo, sale primero el que se inserto antes.
// (Todo esta dentro del .hpp para no tener que cambiar los comandos de compilacion)
class AgendaEventos {
private:
    NodoEvento* cabeza;
    int cantidad;

public:
    AgendaEventos() : cabeza(nullptr), cantidad(0) {}

    ~AgendaEventos() {
        while (cabeza != nullptr) {
            NodoEvento* borrar = cabeza;
            cabeza = cabeza->siguiente;
            delete borrar;
        }
    }

    // La agenda no se copia (evita que dos objetos compartan los mismos nodos)
    AgendaEventos(const AgendaEventos&) = delete;
    AgendaEventos& operator=(const AgendaEventos&) = delete;

    void insertar(const Evento& e) {
        NodoEvento* nuevo = new NodoEvento(e);

        // Caso 1: lista vacia o el nuevo evento es antes que el primero
        if (cabeza == nullptr || e.tiempo < cabeza->dato.tiempo) {
            nuevo->siguiente = cabeza;
            cabeza = nuevo;
        } else {
            // Caso 2: avanzar mientras el siguiente evento sea del mismo tiempo o anterior
            NodoEvento* actual = cabeza;
            while (actual->siguiente != nullptr && actual->siguiente->dato.tiempo <= e.tiempo) {
                actual = actual->siguiente;
            }
            nuevo->siguiente = actual->siguiente;
            actual->siguiente = nuevo;
        }
        cantidad++;
    }

    // Saca y devuelve el evento mas proximo. Se debe verificar !estaVacia() antes.
    Evento sacarSiguiente() {
        NodoEvento* primero = cabeza;
        Evento e = primero->dato;
        cabeza = cabeza->siguiente;
        delete primero;
        cantidad--;
        return e;
    }

    bool estaVacia() const { return cabeza == nullptr; }
    int tamano() const { return cantidad; }
};

#endif