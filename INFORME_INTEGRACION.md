# Informe de Integración y Compatibilidad: TADs y Simulación

Este documento recopila la auditoría técnica, la solución de coexistencia entre el estilo procedimental (tipo C) y orientado a objetos (C++), los comandos de verificación y los resultados oficiales obtenidos en la rama `fix/compatibilidad-tad`.

---

## 1. Mensaje para el Equipo de Trabajo

Puedes compartir este texto directamente en el canal de comunicación del equipo:

> **Hola equipo 👋**
> 
> Hice una revisión e integración en una rama aparte llamada `fix/compatibilidad-tad` para que las implementaciones de todos puedan compilar juntas sin errores y sin que nadie tenga que rehacer su trabajo:
> 
> 1. **Sobre los TADs estilo C (Juan Daniel):**
>    - Se conservaron intactas todas las funciones procedimentales y estructuras (`pacienteIniciar`, `compararPrioridad`, `compararLlegada`, `colaIniciar`, `colaEncolar`, `colaDesencolar`, `Bolsa`, `Iterador`).
>    - Se añadió el calificador `inline` en las funciones de cabecera de `ColaFIFO.hpp` para evitar errores de enlace (*multiple definition*) al compilar varios archivos a la vez.
> 
> 2. **Compatibilidad con las Colas de Prioridad (Bruno) y el Simulador (Brithny):**
>    - El `struct Paciente` ahora mantiene sincronizados tanto los atributos C (`nivelEfectivo`, `llegada`) como los nombres requeridos por el simulador y la batería de pruebas (`urgencia`, `tiempoLlegada`).
>    - Se habilitó la sobrecarga del constructor y el método `tieneMayorPrioridadQue`, el cual internamente utiliza la misma regla estricta que `compararPrioridad`.
> 
> 3. **Verificación realizada:**
>    - Pasó la prueba de equivalencia algorítmica al 100% (`tests/test_comparacion_colas.cpp`).
>    - El ejecutable principal (`simulador.exe`) compila sin advertencias y corre las opciones 1, 2 y 3 del menú.
> 
> Pueden descargar y revisar la rama con:
> ```bash
> git fetch origin
> git checkout fix/compatibilidad-tad
> ```

---

## 2. Diagnóstico del Conflicto Detectado en `develop`

Al analizar los últimos commits en la rama `develop`:
* **Integrante 1 (Juan Daniel):** Adoptó un paradigma procedimental tipo C en `Paciente.hpp` y `ColaFIFO.hpp` (eliminó constructores y métodos miembros, usó arreglos estáticos de `char`, punteros `void*` y funciones externas como `pacienteIniciar` y `compararPrioridad`).
* **Integrante 2 (Bruno) e Integrante 3 (Brithny):** Desarrollaron `MonticuloBinario.cpp`, `ListaOrdenada.cpp`, `Simulador.cpp` y `main.cpp` asumiendo la interfaz original de C++ (`p.tieneMayorPrioridadQue()`, `p.urgencia`, `p.tiempoLlegada`, constructores y métodos `.encolar()` / `.desencolar()` en colas).
* **Prueba Unitaria (`test_comparacion_colas.cpp`):** Esperaba acceder directamente a `p.urgencia`, `p.tiempoLlegada` e instanciar pacientes mediante constructor con parámetros.

Esta falta de acople impedía la compilación directa de cualquiera de los ejecutables del proyecto.

---

## 3. Solución Técnica Implementada (Coexistencia de Paradigmas)

En lugar de imponer un solo paradigma o forzar a un integrante a reescribir su código, se implementó una **capa de compatibilidad dual**:

1. **`src/Paciente.hpp` y `src/Paciente.cpp`:**
   - Se mantuvieron todos los campos definidos por Juan Daniel: `nivelOriginal`, `nivelEfectivo`, `llegada`, `duracion`, `inicio`, `fin`, `diagnostico`.
   - Se agregaron los campos `urgencia` y `tiempoLlegada`, manteniéndolos sincronizados.
   - Se proveyó un constructor de inicialización por defecto y parametrizado para C++, junto con la función C `pacienteIniciar`.
   - Se implementó `p.tieneMayorPrioridadQue(otro)` en sincronía con la lógica de `compararPrioridad(const void*, const void*)`.

2. **`src/ColaFIFO.hpp`:**
   - Se preservó el nodo con `void* dato` y las funciones globales C (`colaIniciar`, `colaEncolar`, `colaDesencolar`, `colaTamano`, etc.), agregando `inline` para evitar conflictos de enlazado.
   - Se agregaron métodos miembros de conveniencia (`encolar`, `desencolar`, `estaVacia`, `tamano`) que permiten utilizar la cola de manera orientada a objetos en el simulador sin romper la estructura interna.

3. **`src/Simulador.cpp`:**
   - Se incluyó `<stdexcept>` para dar soporte a `std::invalid_argument`.
   - En el proceso de envejecimiento (*aging*), se sincroniza tanto `urgencia` como `nivelEfectivo`.

---

## 4. Guía de Compilación y Ejecución

Todos los comandos se ejecutan desde la raíz del repositorio:

### A. Prueba de Equivalencia de Colas (100 Pacientes)
```bash
# En Windows (MinGW / GCC):
g++ -std=c++17 tests/test_comparacion_colas.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp -o tests/test_colas.exe
./tests/test_colas.exe

# En Linux / macOS:
g++ -std=c++17 tests/test_comparacion_colas.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp -o tests/test_colas.out
./tests/test_colas.out
```

### B. Simulador Completo (Dataset de Pacientes)
```bash
# En Windows:
g++ -std=c++17 src/main.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp src/Simulador.cpp -o simulador.exe
./simulador.exe

# En Linux / macOS:
g++ -std=c++17 src/main.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp src/Simulador.cpp -o simulador.out
./simulador.out
```

---

## 5. Resultados de las Pruebas

### Resultado de la Prueba Unitaria:
```text
====================================================
  INICIANDO PLAN DE PRUEBAS: EQUIVALENCIA DE COLAS  
====================================================

[1] Insertando 100 pacientes en ambas estructuras...
[2] Carga completada exitosamente.

[3] Extrayendo y verificando orden exacto de atencion...

>>> RESULTADO: TODAS LAS PRUEBAS PASARON EXITOSAMENTE (100/100).
>>> Ambas estructuras son funcionalmente equivalentes.
```

### Reporte de Simulación (Opciones 1 y 2):
```text
========== REPORTE DE LA SIMULACION ==========
Consultorios:               3
Pacientes atendidos:        6
Tiempo total simulado:      29 min
Espera promedio:            3.67 min
Espera maxima:              13 min
Ascensos por aging:         0

Espera por nivel de urgencia (urgencia original):
Nivel   Atendidos   Promedio    Maxima
1       2           3.50        7
2       1           2.00        2
3       1           0.00        0
4       1           0.00        0
5       1           13.00       13
==============================================
```
*(Tanto Montículo Binario como Lista Ordenada producen exactamente los mismos tiempos).*

### Comparativa de Rendimiento Empírico (Opción 3):
```text
[B] Insertar y extraer N pacientes (tiempos en ms)
N       Heap insertar   Lista insertar  Heap extraer    Lista extraer
1000    0.183 ms        2.293 ms        0.552 ms        0.038 ms
5000    0.943 ms        64.587 ms       3.405 ms        0.269 ms
10000   2.295 ms        291.957 ms      7.522 ms        0.650 ms
20000   3.840 ms        1471.674 ms     17.171 ms       1.001 ms

Lectura: al duplicar N, insertar en la lista tarda ~4 veces más (O(n) por inserción, O(n^2) total).
En el heap crece logarítmicamente (O(log n) por inserción).
```
