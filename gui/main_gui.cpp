#include <QApplication>
#include "VentanaPrincipal.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);      // arranca Qt
    VentanaPrincipal ventana;
    ventana.show();
    return app.exec();                 // bucle de eventos de la ventana
}