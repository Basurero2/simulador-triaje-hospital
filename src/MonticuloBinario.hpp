#ifndef MONTICULO_BINARIO_HPP
#define MONTICULO_BINARIO_HPP

// Funciones del montículo. Se llaman desde cpInsertar, cpExtraer, etc. (ColaPrioridad.hpp).
// flotar() y hundir() son pasos internos y por eso no se declaran aqui.
struct ColaPrioridad;

void m_iniciar(ColaPrioridad* q);
void m_insertar(ColaPrioridad* q, void* x);
void* m_extraer(ColaPrioridad* q);
void m_reordenar(ColaPrioridad* q);
void m_paraCada(ColaPrioridad* q, void (*f)(void*, void*), void* contexto);
void m_liberar(ColaPrioridad* q);

#endif
