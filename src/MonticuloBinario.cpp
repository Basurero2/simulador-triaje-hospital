#include "ColaPrioridad.hpp"

// Montículo binario en un arreglo dinamico de punteros.
// Posicion 0 = elemento de mayor prioridad. Padre de i: (i-1)/2. Hijos de i: 2i+1 y 2i+2.

static void intercambiar(void** a, void** b) {
    void* aux = *a;
    *a = *b;
    *b = aux;
}

// Sube el elemento i mientras gane a su padre.
static void flotar(ColaPrioridad* q, int i) {
    while (i > 0 && cpAntes(q, q->datos[i], q->datos[(i - 1) / 2])) {
        intercambiar(&q->datos[i], &q->datos[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Baja el elemento i intercambiandolo con su mejor hijo mientras el hijo gane.
static void hundir(ColaPrioridad* q, int i) {
    while (2 * i + 1 < q->cant) {
        int m = 2 * i + 1;                                                   // hijo izquierdo
        if (m + 1 < q->cant && cpAntes(q, q->datos[m + 1], q->datos[m])) {
            m = m + 1;                                                       // el derecho es mejor
        }
        if (!cpAntes(q, q->datos[m], q->datos[i])) break;                    // ya esta bien ubicado
        intercambiar(&q->datos[i], &q->datos[m]);
        i = m;
    }
}

// Cuando el arreglo se llena: se reserva uno del doble de tamano, se copia y se libera el anterior.
static void crecer(ColaPrioridad* q) {
    int nuevaCap = q->cap * 2;
    void** nuevo = new void*[nuevaCap];
    for (int i = 0; i < q->cant; i++) nuevo[i] = q->datos[i];
    delete[] q->datos;
    q->datos = nuevo;
    q->cap = nuevaCap;
}

void m_iniciar(ColaPrioridad* q) {
    q->cap = 16;
    q->datos = new void*[q->cap];
    q->cant = 0;
}

void m_insertar(ColaPrioridad* q, void* x) {
    if (q->cant == q->cap) crecer(q);
    q->datos[q->cant] = x;      // se coloca al final...
    q->cant++;
    flotar(q, q->cant - 1);     // ...y sube hasta su lugar
}

void* m_extraer(ColaPrioridad* q) {
    if (q->cant == 0) return NULL;
    void* raiz = q->datos[0];
    q->cant--;
    if (q->cant > 0) {
        q->datos[0] = q->datos[q->cant];   // el ultimo pasa a la raiz...
        hundir(q, 0);                      // ...y baja hasta su lugar
    }
    return raiz;
}

// Reconstruye el montículo hundiendo desde la mitad hacia la raiz: O(n).
void m_reordenar(ColaPrioridad* q) {
    for (int i = q->cant / 2 - 1; i >= 0; i--) hundir(q, i);
}

void m_paraCada(ColaPrioridad* q, void (*f)(void*, void*), void* contexto) {
    for (int i = 0; i < q->cant; i++) f(q->datos[i], contexto);
}

void m_liberar(ColaPrioridad* q) {
    delete[] q->datos;
    q->datos = NULL;
    q->cap = 0;
}
