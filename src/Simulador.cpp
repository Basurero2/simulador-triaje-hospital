#include "Simulador.hpp"
#include <iostream>

Simulador::Simulador() : tiempoActual(0) {}

void Simulador::cargarDatosDesdeCSV(const std::string& rutaCSV) {
    // TODO: Brithny (Integrante 3) - Implementar lectura y parseo del archivo CSV
    std::cout << "[Simulador] Cargando pacientes desde: " << rutaCSV << "...\n";
}

void Simulador::ejecutar(ColaPrioridad& colaTriaje) {
    // TODO: Brithny (Integrante 3) - Implementar motor de eventos, aging y atencion
    std::cout << "[Simulador] Ejecutando motor de eventos discretos...\n";
}

void Simulador::mostrarReporteEstadistico() const {
    // TODO: Brithny (Integrante 3) - Mostrar tabla de tiempos de espera promedio
    std::cout << "[Simulador] Generando reporte estadistico final...\n";
}