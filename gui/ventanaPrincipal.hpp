#pragma once
#include <QMainWindow>
#include <QString>
#include "Simulador.hpp"

class QComboBox;
class QCheckBox;
class QPushButton;
class QSlider;
class QTimer;
class QLabel;
class QTableWidget;
class QPlainTextEdit;
class QTabWidget;
class GraficoBarras;

class VentanaPrincipal : public QMainWindow {
    Q_OBJECT
public:
    VentanaPrincipal();
    ~VentanaPrincipal() override;

private slots:
    void alAbrirCSV();
    void alPreparar();
    void alPaso();
    void alReproducir();
    void alEjecutarCompleta();
    void alCompararAging();
    void alCompararEstructuras();

private:
    void construirInterfaz();
    QWidget* crearPestanaSimulacion();
    QWidget* crearPestanaReporte();
    QWidget* crearPestanaAging();
    QWidget* crearPestanaComparacion();
    bool cargarCSV(const QString& ruta);
    void darPaso();
    void refrescarVista();
    void terminarSimulacion();
    void actualizarBotones();
    TipoCola tipoElegido() const;
    int agingElegido() const;
    QString textoReporte();

    Simulador sim;          // el simulador de siempre
    bool csvCargado;
    bool enCurso;           // true entre simuladorPreparar y simuladorFinalizar
    QTimer* temporizador;

    // Controles superiores
    QComboBox* cmbEstructura;
    QCheckBox* chkAging;
    QPushButton* btnAbrir;
    QPushButton* btnPreparar;
    QPushButton* btnPaso;
    QPushButton* btnReproducir;
    QPushButton* btnCompleta;
    QSlider* sldVelocidad;
    QTabWidget* pestanas;

    // Pestana Simulacion
    QLabel* lblReloj;
    QLabel* lblEvento;
    QLabel* lblContadores;
    QTableWidget* tblEspera;
    QLabel* lblConsultorio[NUM_CONSULTORIOS];
    QLabel* lblFila[NUM_CONSULTORIOS];
    QPlainTextEdit* txtRegistro;

    // Pestana Reporte
    QPlainTextEdit* txtReporte;

    // Pestana Aging
    QTableWidget* tblAging;
    GraficoBarras* grafAging;
    QLabel* lblAging;

    // Pestana Comparacion
    QTableWidget* tblComparacion;
    QLabel* lblComparacion;
};