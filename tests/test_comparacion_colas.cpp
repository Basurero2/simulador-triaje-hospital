// Pruebas de las estructuras. Compilar y ejecutar:
//   g++ -std=c++17 tests/test_comparacion_colas.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp -o tests/test_colas.exe
//   ./tests/test_colas.exe
#include <iostream>
#include <cassert>
#include <cstdlib>
#include "../src/Paciente.hpp"
#include "../src/ColaPrioridad.hpp"
#include "../src/ColaFIFO.hpp"
#include "../src/Iterador.hpp"
#include "../src/Bolsa.hpp"
#include "../src/AgendaEventos.hpp"

const int TOTAL_PACIENTES = 100;

// 1) Heap y lista deben entregar exactamente el mismo orden (100 pacientes, semilla fija)
void probarEquivalencia() {
    Paciente lote[TOTAL_PACIENTES];
    std::srand(42);
    for (int i = 0; i < TOTAL_PACIENTES; i++) {
        pacienteIniciar(&lote[i], i + 1, "P", 1 + std::rand() % 5, std::rand() % 30, 10, 1);
    }
    ColaPrioridad heap, lista;
    cpIniciar(&heap, MONTICULO, compararPrioridad);
    cpIniciar(&lista, LISTA, compararPrioridad);
    for (int i = 0; i < TOTAL_PACIENTES; i++) {
        cpInsertar(&heap, &lote[i]);
        cpInsertar(&lista, &lote[i]);
    }
    assert(cpTamano(&heap) == TOTAL_PACIENTES);
    assert(cpTamano(&lista) == TOTAL_PACIENTES);

    Paciente* anterior = NULL;
    while (!cpVacia(&heap)) {
        Paciente* a = (Paciente*)cpExtraer(&heap);
        Paciente* b = (Paciente*)cpExtraer(&lista);
        assert(a == b);                                          // mismo paciente en cada paso
        if (anterior != NULL) assert(compararPrioridad(anterior, a) < 0);   // orden estricto
        anterior = a;
    }
    assert(cpVacia(&lista));
    cpLiberar(&heap);
    cpLiberar(&lista);
    std::cout << "  [OK] Equivalencia monticulo / lista (100 pacientes)\n";
}

// 2) Cambiar prioridades y reordenar (lo que hace el aging) en ambas estructuras
void probarReordenar() {
    Paciente lote[TOTAL_PACIENTES];
    std::srand(7);
    for (int i = 0; i < TOTAL_PACIENTES; i++) {
        pacienteIniciar(&lote[i], i + 1, "P", 1 + std::rand() % 5, i, 10, 1);
    }
    ColaPrioridad heap, lista;
    cpIniciar(&heap, MONTICULO, compararPrioridad);
    cpIniciar(&lista, LISTA, compararPrioridad);
    for (int i = 0; i < TOTAL_PACIENTES; i++) {
        cpInsertar(&heap, &lote[i]);
        cpInsertar(&lista, &lote[i]);
    }
    for (int i = 0; i < TOTAL_PACIENTES; i += 3) {               // subir de nivel a algunos
        if (lote[i].nivelEfectivo > 1) lote[i].nivelEfectivo--;
    }
    cpReordenar(&heap);
    cpReordenar(&lista);
    while (!cpVacia(&heap)) {
        assert(cpExtraer(&heap) == cpExtraer(&lista));
    }
    cpLiberar(&heap);
    cpLiberar(&lista);
    std::cout << "  [OK] Reordenar tras cambiar prioridades (aging)\n";
}

// 3) Desempate: igual nivel e igual llegada -> por id; extraer de cola vacia -> NULL
void probarBordes() {
    Paciente a, b, c;
    pacienteIniciar(&a, 3, "A", 2, 10, 5, 1);
    pacienteIniciar(&b, 1, "B", 2, 10, 5, 1);
    pacienteIniciar(&c, 2, "C", 1, 50, 5, 1);                    // mas urgente aunque llego despues
    for (int t = 0; t < 2; t++) {
        ColaPrioridad q;
        cpIniciar(&q, t == 0 ? MONTICULO : LISTA, compararPrioridad);
        assert(cpExtraer(&q) == NULL);                           // vacia -> NULL
        cpInsertar(&q, &a);
        cpInsertar(&q, &b);
        cpInsertar(&q, &c);
        assert(cpVer(&q) == &c);
        assert(cpExtraer(&q) == &c);
        assert(cpExtraer(&q) == &b);                             // empate: gana el id menor
        assert(cpExtraer(&q) == &a);
        assert(cpVacia(&q));
        cpInsertar(&q, &a);                                      // reutilizar tras vaciar
        assert(cpTamano(&q) == 1);
        cpLiberar(&q);
    }
    std::cout << "  [OK] Desempate por id, cola vacia y reutilizacion\n";
}

// 4) Montículo con mas elementos que su capacidad inicial (debe crecer)
void probarCrecimiento() {
    const int N = 1000;
    Paciente* lote = new Paciente[N];
    for (int i = 0; i < N; i++) pacienteIniciar(&lote[i], i + 1, "P", 1 + (i * 7) % 5, N - i, 10, 1);
    ColaPrioridad q;
    cpIniciar(&q, MONTICULO, compararPrioridad);
    for (int i = 0; i < N; i++) cpInsertar(&q, &lote[i]);
    Paciente* anterior = NULL;
    while (!cpVacia(&q)) {
        Paciente* p = (Paciente*)cpExtraer(&q);
        if (anterior != NULL) assert(compararPrioridad(anterior, p) < 0);
        anterior = p;
    }
    cpLiberar(&q);
    delete[] lote;
    std::cout << "  [OK] Crecimiento del arreglo del monticulo (1000 elementos)\n";
}

// 5) Cola FIFO con capacidad, iterador y bolsa
void probarFifoIteradorBolsa() {
    Paciente p1, p2, p3;
    pacienteIniciar(&p1, 1, "A", 3, 1, 5, 1);
    pacienteIniciar(&p2, 2, "B", 3, 2, 5, 1);
    pacienteIniciar(&p3, 3, "C", 3, 3, 5, 1);

    ColaFIFO f;
    colaIniciar(&f, 2);
    assert(colaDesencolar(&f) == NULL);                          // vacia -> NULL
    assert(colaEncolar(&f, &p1) == 1);
    assert(colaEncolar(&f, &p2) == 1);
    assert(colaLlena(&f));
    assert(colaEncolar(&f, &p3) == 0);                           // llena -> 0, no cambia
    assert(colaTamano(&f) == 2);

    Iterador it;                                                 // recorre sin sacar
    iteradorIniciar(&it, &f);
    assert(iteradorHaySiguiente(&it));
    assert(iteradorSiguiente(&it) == &p1);
    assert(iteradorSiguiente(&it) == &p2);
    assert(!iteradorHaySiguiente(&it));
    assert(colaTamano(&f) == 2);

    assert(colaDesencolar(&f) == &p1);                           // orden FIFO
    assert(colaDesencolar(&f) == &p2);
    assert(colaVacia(&f));
    assert(colaEncolar(&f, &p3) == 1);                           // reutilizable
    colaLiberar(&f);
    assert(colaVacia(&f));

    Bolsa b;
    bolsaIniciar(&b);
    bolsaAgregar(&b, 5);
    bolsaAgregar(&b, 5);
    bolsaAgregar(&b, 2);
    assert(bolsaContar(&b, 5) == 2);
    assert(bolsaContar(&b, 2) == 1);
    assert(bolsaContar(&b, 9) == 0);
    bolsaLiberar(&b);
    std::cout << "  [OK] ColaFIFO, Iterador y Bolsa\n";
}

// 6) La agenda entrega los eventos por tiempo; en empate: fin, llegada, aging; luego orden de programacion
void probarAgenda() {
    Paciente p;
    pacienteIniciar(&p, 1, "A", 3, 0, 5, 1);
    Agenda a;
    agendaIniciar(&a);
    agendaProgramar(&a, 30, EV_ENVEJECIMIENTO, NULL, -1);
    agendaProgramar(&a, 10, EV_LLEGADA, &p, -1);
    agendaProgramar(&a, 10, EV_FIN_ATENCION, &p, 2);
    agendaProgramar(&a, 10, EV_LLEGADA, &p, -1);
    agendaProgramar(&a, 5, EV_LLEGADA, &p, -1);
    assert(agendaTamano(&a) == 5);
    assert(agendaVerSiguiente(&a)->tiempo == 5);
    assert(agendaTamano(&a) == 5);                               // ver no modifica

    int tiempos[5]  = {5, 10, 10, 10, 30};
    int tipos[5]    = {EV_LLEGADA, EV_FIN_ATENCION, EV_LLEGADA, EV_LLEGADA, EV_ENVEJECIMIENTO};
    long ordenes[5] = {4, 2, 1, 3, 0};
    for (int i = 0; i < 5; i++) {
        Evento* e = agendaExtraerSiguiente(&a);
        assert(e->tiempo == tiempos[i]);
        assert(e->tipo == tipos[i]);
        assert(e->orden == ordenes[i]);
        delete e;
    }
    assert(agendaVacia(&a));

    agendaProgramar(&a, 1, EV_LLEGADA, &p, -1);                  // eventos pendientes se liberan solos
    agendaLiberar(&a);
    std::cout << "  [OK] Agenda de eventos (orden y desempate)\n";
}

int main() {
    std::cout << "Ejecutando pruebas...\n";
    probarEquivalencia();
    probarReordenar();
    probarBordes();
    probarCrecimiento();
    probarFifoIteradorBolsa();
    probarAgenda();
    std::cout << "\n>>> RESULTADO: TODAS LAS PRUEBAS PASARON EXITOSAMENTE.\n";
    return 0;
}
