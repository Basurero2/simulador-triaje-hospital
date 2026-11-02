# Simulador de Triaje y Atención Hospitalaria en C++

Sistema de simulación por eventos discretos orientado a la gestión de salas de emergencia hospitalaria mediante clasificación de triaje (niveles de urgencia 1 al 5). El software evalúa y compara el rendimiento computacional entre una Cola de Prioridad basada en Montículo Binario (*Binary Heap*) y una basada en Lista Enlazada Ordenada, incorporando mecanismos de envejecimiento (*aging*) para prevenir la inanición de pacientes de menor gravedad.

**Asignatura:** Algoritmos y Estructuras de Datos (UNMSM)  
**Lenguaje:** C++ (Estándar C++17)  
**Compiladores compatibles:** GCC (`g++` 9+) / Clang (10+)  

---

## 👥 Integrantes y Distribución de Responsabilidades (Entrega 1)

| Integrante | Nombre y Apellidos | Archivos Asignados (Software) | Responsabilidad Técnica | Responsabilidad Documental (Informe 1) |
| :--- | :--- | :--- | :--- | :--- |
| **Integrante 1** | Juan Daniel Villanueva Soria | `src/Paciente.hpp`<br>`src/Paciente.cpp`<br>`src/ColaFIFO.hpp`<br>`src/Bolsa.hpp`<br>`src/Iterador.hpp` | Implementación de TAD Paciente, Cola FIFO, TAD Bolsa e Iterador. | **Editor General** (compilación final, carátula, índices, formato APA 7) y Especificación de los TADs asignados. |
| **Integrante 2** | Bruno Gabriel Mamani Berrocal | `src/ColaPrioridad.hpp`<br>`src/MonticuloBinario.hpp`<br>`src/MonticuloBinario.cpp`<br>`src/ListaOrdenada.hpp`<br>`src/ListaOrdenada.cpp` | Implementación de Cola de Prioridad con Montículo Binario y Lista Ordenada con comparador. | Especificación técnica de la Cola de Prioridad y Justificación Comparativa Teórica (Criterio 2). |
| **Integrante 3** | Brithny Zolanch de Elizabeth Cruz Matos | `src/AgendaEventos.hpp`<br>`src/Simulador.hpp`<br>`src/Simulador.cpp`<br>`src/main.cpp` | Prototipo y diseño de la Agenda de Eventos y estructura base del motor de simulación. | Redacción del Modelado del Problema (requisitos del sistema, alcance y decisiones de diseño). |
| **Integrante 4** | Juan Carlos Mamani Apaza | `.gitignore`<br>`README.md`<br>`tests/test_comparacion_colas.cpp`<br>`data/pacientes_1000.csv` | Configuración del repositorio Git, estructura de carpetas y Plan de Pruebas de equivalencia (Heap vs. Lista). | Redacción de la Introducción (Contexto, Objetivos), Alcance/Limitaciones y Referencias bibliográficas (APA 7). |

---

## 📁 Estructura del Repositorio

```text
simulador-triaje-cpp/
├── src/                      # Código fuente principal de la aplicación
│   ├── Paciente.hpp          # TAD Paciente (Estructura y cabeceras)
│   ├── Paciente.cpp          # Implementación del TAD Paciente
│   ├── ColaFIFO.hpp          # Cola de atención por consultorio (Lista Enlazada)
│   ├── Bolsa.hpp             # TAD Bolsa (Estructura de colección no ordenada)
│   ├── Iterador.hpp          # Iterador para recorrido secuencial de colecciones
│   ├── ColaPrioridad.hpp     # Interfaz abstracta común para Colas de Prioridad
│   ├── MonticuloBinario.hpp  # Cabecera de Cola de Prioridad con Heap
│   ├── MonticuloBinario.cpp  # Implementación de operaciones del Heap (flotar/hundir)
│   ├── ListaOrdenada.hpp     # Cabecera de Cola de Prioridad con Lista Enlazada
│   ├── ListaOrdenada.cpp     # Implementación de inserción ordenada y extracción
│   ├── AgendaEventos.hpp     # Prototipo y estructura de la Agenda de Eventos
│   ├── Simulador.hpp         # Motor del simulador por eventos discretos
│   ├── Simulador.cpp         # Implementación de lógica de triaje y atención
│   └── main.cpp              # Menú interactivo y punto de entrada (CLI)
│
├── tests/                    # Plan de Pruebas Unitarias
│   └── test_comparacion_colas.cpp  # Prueba de equivalencia funcional (Heap vs Lista)
│
├── docs/                     # Entregables escritos e informes PDF (APA 7)
│   └── Informe1_Triaje_Hospitalario.pdf
│
├── data/                     # Datasets de prueba (Llegadas de pacientes en CSV)
│   └── pacientes_1000.csv
│
├── .gitignore                # Filtro de archivos temporales y compilados
└── README.md                 # Documentación principal del proyecto
```

---

## 📌 Guía Rápida para el Docente / Evaluador

Siga estos pasos para clonar el repositorio, compilar el simulador principal y ejecutar la batería de pruebas de equivalencia algorítmica.

### 1. Clonar y Acceder al Directorio

```bash
git clone https://github.com/Basurero2/simulador-triaje-hospital.git
cd simulador-triaje-hospital
```

*(Nota: Todos los comandos de compilación y ejecución deben ejecutarse desde la raíz del proyecto).*

### 2. Ejecución de Pruebas de Equivalencia (Lote de 100 Pacientes)
Valida que la Cola de Prioridad con Montículo Binario y la basada en Lista Ordenada produzcan exactamente el mismo orden de atención médica y respeten la estabilidad FIFO ante prioridades idénticas:

```bash
# En Windows (CMD / PowerShell / Git Bash):
g++ -std=c++17 tests/test_comparacion_colas.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp -o tests/test_colas.exe
./tests/test_colas.exe

# En Linux / macOS:
g++ -std=c++17 tests/test_comparacion_colas.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp -o tests/test_colas.out
./tests/test_colas.out
```

**Criterio de éxito:** La consola reportará `RESULTADO: TODAS LAS PRUEBAS PASARON EXITOSAMENTE (100/100)`. En caso de discrepancia algorítmica, se detendrá de inmediato mediante `assert`.

### 3. Ejecución del Simulador Completo (Dataset de 1,000 Pacientes)
Ejecuta el flujo completo de llegadas masivas, clasificación, asignación de consultorios y cálculo de tiempos promedio de espera:

```bash
# En Windows:
g++ -std=c++17 src/main.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp src/Simulador.cpp -o simulador.exe
./simulador.exe

# En Linux / macOS:
g++ -std=c++17 src/main.cpp src/Paciente.cpp src/MonticuloBinario.cpp src/ListaOrdenada.cpp src/Simulador.cpp -o simulador.out
./simulador.out
```

---

## 🔄 Flujo de Desarrollo y Gestión de Git (Equipo)

Para garantizar la integridad del código y evitar conflictos de integración durante la semana de desarrollo, el equipo adopta el siguiente protocolo de control de versiones:

### 1. Política de Ramas
* **`main`:** Contiene exclusivamente código estable, probado y verificado. Solo se actualiza al cierre de la entrega.
* **`develop`:** Rama activa de integración diaria donde cada integrante aporta sus clases asignadas.

### 2. Flujo Diario para Integrantes 1, 2 y 3 (Desarrollo en `develop`)

Cada integrante trabaja en sus archivos asignados dentro de `src/` aplicando este ciclo de sincronización:

```bash
# 1. Antes de iniciar la jornada, actualizar cambios del equipo:
git checkout develop
git pull origin develop

# 2. Programar en los archivos .hpp y .cpp correspondientes a su rol.

# 3. Al terminar y verificar que no genera errores de sintaxis, guardar cambios locales:
git add .
git commit -m "feat: avance en metodos de la clase asignada"

# 4. Sincronizar novedades remotas y subir a GitHub:
git pull origin develop
git push origin develop
```

### 3. Protocolo de Integración y Pruebas (Integrante 4)

El Integrante 4 es responsable de validar periódicamente la compatibilidad de las clases subidas por el equipo:

1. **Descarga de código:** Ejecuta `git pull origin develop` para traer las implementaciones de `MonticuloBinario.cpp` y `ListaOrdenada.cpp`.
2. **Ejecución del test unitario:** Compila y corre `tests/test_comparacion_colas.cpp` para comprobar la equivalencia funcional. Si se detecta un error de desempate o punteros, se reporta al Integrante 2 con el ID del paciente discordante.
3. **Pase a producción (Cierre de entrega):** Cuando todas las estructuras compilan y pasan las pruebas al 100%, fusiona la versión validada hacia la rama principal:
   ```bash
   git checkout main
   git merge develop
   git push origin main
   ```
