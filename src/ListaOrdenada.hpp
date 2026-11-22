#ifndef LISTA_ORDENADA_HPP
#define LISTA_ORDENADA_HPP

// Funciones de la lista ordenada. Se llaman desde cpInsertar, cpExtraer, etc. (ColaPrioridad.hpp).
struct ColaPrioridad;

void l_iniciar(ColaPrioridad* q);
void l_insertar(ColaPrioridad* q, void* x);
void* l_extraer(ColaPrioridad* q);
void l_reordenar(ColaPrioridad* q);
void l_paraCada(ColaPrioridad* q, void (*f)(void*, void*), void* contexto);
void l_liberar(ColaPrioridad* q);

#endif
