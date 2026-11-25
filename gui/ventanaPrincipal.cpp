#include "VentanaPrincipal.hpp"
#include "GraficoBarras.hpp"
#include "Iterador.hpp"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

// ======================= Auxiliares (solo de este archivo) =======================
namespace {

// Captura lo que el nucleo imprime con std::cout (el reporte, mensajes de carga)
struct CapturaCout {
    std::ostringstream buffer;
    std::streambuf* anterior;
    CapturaCout() : anterior(std::cout.rdbuf(buffer.rdbuf())) {}
    ~CapturaCout() { std::cout.rdbuf(anterior); }
    QString texto() const { return QString::fromStdString(buffer.str()); }
};

// Colores del triaje de Manchester
QColor colorNivel(int n) {
    switch (n) {
        case 1: return QColor(211, 47, 47);     // rojo
        case 2: return QColor(245, 124, 0);     // naranja
        case 3: return QColor(251, 192, 45);    // amarillo
        case 4: return QColor(56, 142, 60);     // verde
        default: return QColor(25, 118, 210);   // azul
    }
}

QTableWidgetItem* celda(const QString& texto) {
    QTableWidgetItem* it = new QTableWidgetItem(texto);
    it->setTextAlignment(Qt::AlignCenter);
    return it;
}

// Para cpParaCada: junta los pacientes de la cola en un std::vector
void recolectar(void* elemento, void* contexto) {
    std::vector<Paciente*>* lista = (std::vector<Paciente*>*)contexto;
    lista->push_back((Paciente*)elemento);
}

QString descripcion(const Evento& ev) {
    if (ev.tipo == EV_LLEGADA) {
        return QString("LLEGADA: #%1 %2 (nivel %3)")
            .arg(ev.paciente->id)
            .arg(QString::fromUtf8(ev.paciente->nombre))
            .arg(ev.paciente->nivelOriginal);
    }
    if (ev.tipo == EV_FIN_ATENCION) {
        return QString("FIN_ATENCION: termina #%1 en consultorio %2")
            .arg(ev.paciente->id)
            .arg(ev.consultorio + 1);
    }
    return QString("ENVEJECIMIENTO: revisión de la espera");
}

// Esperas por nivel original (indice 0 = global)
struct EstadNiveles {
    int cant[NIVELES + 1];
    long suma[NIVELES + 1];
    int maxi[NIVELES + 1];
    int ascensos;
    int tiempoTotal;
};

EstadNiveles calcular(const Simulador& s) {
    EstadNiveles e;
    for (int n = 0; n <= NIVELES; n++) {
        e.cant[n] = 0;
        e.suma[n] = 0;
        e.maxi[n] = 0;
    }
    for (int i = 0; i < s.numPacientes; i++) {
        const Paciente& p = s.pacientes[i];
        if (p.inicio < 0) continue;
        int espera = p.inicio - p.llegada;
        int n = p.nivelOriginal;
        e.cant[n]++;
        e.suma[n] += espera;
        if (espera > e.maxi[n]) e.maxi[n] = espera;
        e.cant[0]++;
        e.suma[0] += espera;
        if (espera > e.maxi[0]) e.maxi[0] = espera;
    }
    e.ascensos = s.ascensos;
    e.tiempoTotal = s.tiempoFinal;
    return e;
}

double promedio(const EstadNiveles& e, int n) {
    return e.cant[n] ? double(e.suma[n]) / e.cant[n] : 0.0;
}

// Igual que medirCola de main.cpp: inserta y extrae todo el lote midiendo tiempo y comparaciones
void medirCola(TipoCola tipo, Paciente* lote, int n, double* msIns, double* msExt, long* comp) {
    ColaPrioridad q;
    cpIniciar(&q, tipo, compararPrioridad);
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < n; i++) cpInsertar(&q, &lote[i]);
    auto t1 = std::chrono::steady_clock::now();
    while (!cpVacia(&q)) cpExtraer(&q);
    auto t2 = std::chrono::steady_clock::now();
    *msIns = std::chrono::duration<double, std::milli>(t1 - t0).count();
    *msExt = std::chrono::duration<double, std::milli>(t2 - t1).count();
    *comp = q.comparaciones;
    cpLiberar(&q);
}

}  // namespace

// ======================= Construccion =======================

VentanaPrincipal::VentanaPrincipal() : csvCargado(false), enCurso(false) {
    simuladorIniciar(&sim);
    temporizador = new QTimer(this);
    construirInterfaz();
    connect(temporizador, &QTimer::timeout, this, &VentanaPrincipal::darPaso);

    // Busca el CSV junto al .exe y, si no, en la carpeta desde donde se ejecuto
    QStringList candidatos;
    candidatos << QCoreApplication::applicationDirPath() + "/data/pacientes_1000.csv"
               << QString("data/pacientes_1000.csv");
    for (int i = 0; i < candidatos.size() && !csvCargado; i++) {
        if (QFile::exists(candidatos[i])) cargarCSV(candidatos[i]);
    }
    if (!csvCargado) {
        statusBar()->showMessage("No se encontró data/pacientes_1000.csv: use «Abrir CSV…»");
    }
    actualizarBotones();
}

VentanaPrincipal::~VentanaPrincipal() {
    if (enCurso) simuladorFinalizar(&sim);
    simuladorLiberar(&sim);
}

void VentanaPrincipal::construirInterfaz() {
    setWindowTitle("Simulador de triaje hospitalario");
    resize(1050, 700);

    cmbEstructura = new QComboBox;
    cmbEstructura->addItem("Montículo binario");
    cmbEstructura->addItem("Lista ordenada");
    chkAging = new QCheckBox("Aging");
    chkAging->setChecked(true);
    btnAbrir = new QPushButton("Abrir CSV…");
    btnPreparar = new QPushButton("Preparar");
    btnPaso = new QPushButton("Paso");
    btnReproducir = new QPushButton("Reproducir");
    btnCompleta = new QPushButton("Ejecutar completa");
    sldVelocidad = new QSlider(Qt::Horizontal);
    sldVelocidad->setRange(1, 10);      // 1 = lento, 10 = rapido
    sldVelocidad->setValue(5);

    QHBoxLayout* barra = new QHBoxLayout;
    barra->addWidget(new QLabel("Estructura:"));
    barra->addWidget(cmbEstructura);
    barra->addWidget(chkAging);
    barra->addWidget(btnAbrir);
    barra->addWidget(btnPreparar);
    barra->addWidget(btnPaso);
    barra->addWidget(btnReproducir);
    barra->addWidget(btnCompleta);
    barra->addWidget(new QLabel("Velocidad:"));
    barra->addWidget(sldVelocidad);

    pestanas = new QTabWidget;
    pestanas->addTab(crearPestanaSimulacion(), "Simulación");
    pestanas->addTab(crearPestanaReporte(), "Reporte");
    pestanas->addTab(crearPestanaAging(), "Aging con y sin");
    pestanas->addTab(crearPestanaComparacion(), "Comparación");

    QWidget* centro = new QWidget;
    QVBoxLayout* capa = new QVBoxLayout(centro);
    capa->addLayout(barra);
    capa->addWidget(pestanas);
    setCentralWidget(centro);

    connect(btnAbrir, &QPushButton::clicked, this, &VentanaPrincipal::alAbrirCSV);
    connect(btnPreparar, &QPushButton::clicked, this, &VentanaPrincipal::alPreparar);
    connect(btnPaso, &QPushButton::clicked, this, &VentanaPrincipal::alPaso);
    connect(btnReproducir, &QPushButton::clicked, this, &VentanaPrincipal::alReproducir);
    connect(btnCompleta, &QPushButton::clicked, this, &VentanaPrincipal::alEjecutarCompleta);
    connect(sldVelocidad, &QSlider::valueChanged, this, [this](int v) {
        temporizador->setInterval(1100 - v * 100);   // entre 1000 ms y 100 ms por evento
    });
}

QWidget* VentanaPrincipal::crearPestanaSimulacion() {
    lblReloj = new QLabel("t = – min");
    QFont fuente = lblReloj->font();
    fuente.setPointSize(14);
    fuente.setBold(true);
    lblReloj->setFont(fuente);
    lblEvento = new QLabel("Sin simulación en curso");
    lblContadores = new QLabel;

    tblEspera = new QTableWidget(0, 5);
    tblEspera->setHorizontalHeaderLabels({"Id", "Nombre", "Nivel", "Original", "Espera (min)"});
    tblEspera->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblEspera->setSelectionMode(QAbstractItemView::NoSelection);
    tblEspera->verticalHeader()->setVisible(false);
    tblEspera->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QGroupBox* cajaEspera = new QGroupBox("Sala de espera (cola de prioridad)");
    QVBoxLayout* capaEspera = new QVBoxLayout(cajaEspera);
    capaEspera->addWidget(tblEspera);

    QVBoxLayout* capaConsultorios = new QVBoxLayout;
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        QGroupBox* caja = new QGroupBox(QString("Consultorio %1").arg(c + 1));
        QVBoxLayout* capa = new QVBoxLayout(caja);
        lblConsultorio[c] = new QLabel("○ Libre");
        lblConsultorio[c]->setWordWrap(true);
        lblFila[c] = new QLabel("Fila: –");
        lblFila[c]->setWordWrap(true);
        capa->addWidget(lblConsultorio[c]);
        capa->addWidget(lblFila[c]);
        capaConsultorios->addWidget(caja);
    }

    txtRegistro = new QPlainTextEdit;
    txtRegistro->setReadOnly(true);
    txtRegistro->setMaximumHeight(130);

    QHBoxLayout* arriba = new QHBoxLayout;
    arriba->addWidget(lblReloj);
    arriba->addWidget(lblEvento, 1);
    arriba->addWidget(lblContadores);

    QHBoxLayout* medio = new QHBoxLayout;
    medio->addWidget(cajaEspera, 3);
    medio->addLayout(capaConsultorios, 2);

    QWidget* pagina = new QWidget;
    QVBoxLayout* capaPagina = new QVBoxLayout(pagina);
    capaPagina->addLayout(arriba);
    capaPagina->addLayout(medio, 1);
    capaPagina->addWidget(txtRegistro);
    return pagina;
}

QWidget* VentanaPrincipal::crearPestanaReporte() {
    txtReporte = new QPlainTextEdit;
    txtReporte->setReadOnly(true);
    txtReporte->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    txtReporte->setPlainText("Termine o ejecute una simulación para ver el reporte.");
    QWidget* pagina = new QWidget;
    QVBoxLayout* capa = new QVBoxLayout(pagina);
    capa->addWidget(txtReporte);
    return pagina;
}

QWidget* VentanaPrincipal::crearPestanaAging() {
    QPushButton* boton = new QPushButton("Comparar con y sin aging (montículo)");
    lblAging = new QLabel("Simula dos veces con los mismos pacientes: con aging y sin aging.");
    lblAging->setWordWrap(true);

    tblAging = new QTableWidget(6, 6);
    tblAging->setHorizontalHeaderLabels(
        {"Nivel", "Atendidos", "Prom. ON (min)", "Máx. ON (min)", "Prom. OFF (min)", "Máx. OFF (min)"});
    tblAging->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblAging->setSelectionMode(QAbstractItemView::NoSelection);
    tblAging->verticalHeader()->setVisible(false);
    tblAging->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblAging->setMaximumHeight(230);

    grafAging = new GraficoBarras;

    QWidget* pagina = new QWidget;
    QVBoxLayout* capa = new QVBoxLayout(pagina);
    capa->addWidget(boton);
    capa->addWidget(lblAging);
    capa->addWidget(tblAging);
    capa->addWidget(grafAging, 1);
    connect(boton, &QPushButton::clicked, this, &VentanaPrincipal::alCompararAging);
    return pagina;
}

QWidget* VentanaPrincipal::crearPestanaComparacion() {
    QPushButton* boton = new QPushButton("Medir montículo contra lista (puede tardar un par de segundos)");
    lblComparacion = new QLabel("Inserta y extrae lotes de 1 000 a 20 000 pacientes con ambas estructuras.");
    lblComparacion->setWordWrap(true);

    tblComparacion = new QTableWidget(4, 7);
    tblComparacion->setHorizontalHeaderLabels({"N", "Heap insertar (ms)", "Lista insertar (ms)",
                                               "Heap extraer (ms)", "Lista extraer (ms)",
                                               "Comp. heap", "Comp. lista"});
    tblComparacion->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblComparacion->setSelectionMode(QAbstractItemView::NoSelection);
    tblComparacion->verticalHeader()->setVisible(false);
    tblComparacion->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QWidget* pagina = new QWidget;
    QVBoxLayout* capa = new QVBoxLayout(pagina);
    capa->addWidget(boton);
    capa->addWidget(lblComparacion);
    capa->addWidget(tblComparacion);
    capa->addStretch(1);
    connect(boton, &QPushButton::clicked, this, &VentanaPrincipal::alCompararEstructuras);
    return pagina;
}

// ======================= Datos y estado =======================

bool VentanaPrincipal::cargarCSV(const QString& ruta) {
    if (enCurso) {
        temporizador->stop();
        btnReproducir->setText("Reproducir");
        simuladorFinalizar(&sim);
        enCurso = false;
    }
    CapturaCout captura;
    int n = simuladorCargarCSV(&sim, ruta.toStdString());
    txtRegistro->setPlainText(captura.texto());
    csvCargado = (n > 0);
    if (csvCargado) {
        statusBar()->showMessage(QString("%1 pacientes cargados desde %2").arg(n).arg(ruta));
    } else {
        statusBar()->showMessage("No se cargó ningún paciente");
    }
    return csvCargado;
}

void VentanaPrincipal::actualizarBotones() {
    btnPreparar->setEnabled(csvCargado);
    btnPaso->setEnabled(csvCargado);
    btnReproducir->setEnabled(csvCargado);
    btnCompleta->setEnabled(csvCargado);
    cmbEstructura->setEnabled(!enCurso);   // estructura y aging se fijan al preparar
    chkAging->setEnabled(!enCurso);
}

TipoCola VentanaPrincipal::tipoElegido() const {
    return (cmbEstructura->currentIndex() == 0) ? MONTICULO : LISTA;
}

int VentanaPrincipal::agingElegido() const {
    return chkAging->isChecked() ? 1 : 0;
}

QString VentanaPrincipal::textoReporte() {
    CapturaCout captura;
    simuladorReporte(&sim);
    return captura.texto();
}

// ======================= Simulacion en vivo =======================

void VentanaPrincipal::alAbrirCSV() {
    QString ruta = QFileDialog::getOpenFileName(this, "Abrir CSV de pacientes", QString(),
                                                "Archivos CSV (*.csv)");
    if (ruta.isEmpty()) return;
    if (!cargarCSV(ruta)) {
        QMessageBox::warning(this, "CSV", "No se pudo cargar ningún paciente de ese archivo.");
    }
    actualizarBotones();
}

void VentanaPrincipal::alPreparar() {
    if (!csvCargado) return;
    temporizador->stop();
    btnReproducir->setText("Reproducir");
    if (enCurso) {
        simuladorFinalizar(&sim);
        enCurso = false;
    }
    if (!simuladorPreparar(&sim, tipoElegido(), agingElegido())) return;
    enCurso = true;
    txtRegistro->clear();
    lblEvento->setText("Listo: pulse Paso o Reproducir");
    refrescarVista();
    actualizarBotones();
}

void VentanaPrincipal::alPaso() {
    if (!enCurso) alPreparar();
    if (enCurso) darPaso();
}

void VentanaPrincipal::alReproducir() {
    if (!enCurso) alPreparar();
    if (!enCurso) return;
    if (temporizador->isActive()) {
        temporizador->stop();
        btnReproducir->setText("Reproducir");
    } else {
        temporizador->start(1100 - sldVelocidad->value() * 100);
        btnReproducir->setText("Pausar");
    }
}

void VentanaPrincipal::darPaso() {
    if (!enCurso) return;
    Evento ev;
    if (!simuladorPaso(&sim, &ev)) {
        terminarSimulacion();
        return;
    }
    lblEvento->setText(descripcion(ev));
    txtRegistro->appendPlainText(QString("t=%1  %2").arg(ev.tiempo).arg(descripcion(ev)));
    refrescarVista();
    if (agendaVacia(&sim.agenda)) terminarSimulacion();
}

void VentanaPrincipal::terminarSimulacion() {
    temporizador->stop();
    btnReproducir->setText("Reproducir");
    simuladorFinalizar(&sim);
    enCurso = false;
    txtReporte->setPlainText(textoReporte());
    lblEvento->setText("Simulación terminada: vea la pestaña Reporte");
    actualizarBotones();
}

void VentanaPrincipal::alEjecutarCompleta() {
    if (!csvCargado) return;
    temporizador->stop();
    btnReproducir->setText("Reproducir");
    if (enCurso) {
        simuladorFinalizar(&sim);
        enCurso = false;
    }
    simuladorEjecutar(&sim, tipoElegido(), agingElegido(), 0);
    txtReporte->setPlainText(textoReporte());
    pestanas->setCurrentIndex(1);   // muestra el reporte
    actualizarBotones();
}

// Redibuja la sala de espera, los consultorios y los contadores (solo se llama con enCurso = true)
void VentanaPrincipal::refrescarVista() {
    int ahora = relojObtener(&sim.reloj);

    // Sala de espera: se leen los elementos de la cola y se ordenan solo para mostrarlos
    std::vector<Paciente*> espera;
    cpParaCada(&sim.cola, recolectar, &espera);
    std::sort(espera.begin(), espera.end(),
              [](Paciente* x, Paciente* y) { return compararPrioridad(x, y) < 0; });

    tblEspera->setRowCount((int)espera.size());
    for (int i = 0; i < (int)espera.size(); i++) {
        const Paciente* p = espera[i];
        QTableWidgetItem* itemNivel = celda(QString::number(p->nivelEfectivo));
        itemNivel->setBackground(colorNivel(p->nivelEfectivo));
        itemNivel->setForeground(p->nivelEfectivo == 3 ? Qt::black : Qt::white);
        tblEspera->setItem(i, 0, celda(QString::number(p->id)));
        tblEspera->setItem(i, 1, new QTableWidgetItem(QString::fromUtf8(p->nombre)));
        tblEspera->setItem(i, 2, itemNivel);
        tblEspera->setItem(i, 3, celda(QString::number(p->nivelOriginal)));
        tblEspera->setItem(i, 4, celda(QString::number(ahora - p->llegada)));
    }

    // Consultorios: quien atiende y su fila FIFO (se recorre con el Iterador, sin sacar a nadie)
    int atendidos = 0;
    for (int c = 0; c < NUM_CONSULTORIOS; c++) {
        const Consultorio& k = sim.consultorios[c];
        atendidos += k.atendidos;
        if (k.ocupado && k.actual != NULL) {
            lblConsultorio[c]->setText(QString("● Atiende #%1 %2 (nivel %3) hasta t = %4")
                                           .arg(k.actual->id)
                                           .arg(QString::fromUtf8(k.actual->nombre))
                                           .arg(k.actual->nivelOriginal)
                                           .arg(k.actual->fin));
        } else {
            lblConsultorio[c]->setText("○ Libre");
        }
        QString fila;
        Iterador it;
        iteradorIniciar(&it, &k.fila);
        while (iteradorHaySiguiente(&it)) {
            Paciente* p = iteradorSiguiente(&it);
            fila += QString("[#%1 N%2] ").arg(p->id).arg(p->nivelEfectivo);
        }
        lblFila[c]->setText(QString("Fila: ") + (fila.isEmpty() ? QString("–") : fila));
    }

    lblReloj->setText(QString("t = %1 min").arg(ahora));
    lblContadores->setText(QString("En espera: %1 | Atendidos: %2 | Ascensos: %3 | Comparaciones: %4")
                               .arg(cpTamano(&sim.cola))
                               .arg(atendidos)
                               .arg(sim.ascensos)
                               .arg(sim.cola.comparaciones));
}

// ======================= Pestana Aging =======================

void VentanaPrincipal::alCompararAging() {
    if (!csvCargado) {
        QMessageBox::information(this, "Aging", "Primero cargue un CSV de pacientes.");
        return;
    }
    if (enCurso) {
        temporizador->stop();
        btnReproducir->setText("Reproducir");
        simuladorFinalizar(&sim);
        enCurso = false;
        actualizarBotones();
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    simuladorEjecutar(&sim, MONTICULO, 1, 0);
    EstadNiveles on = calcular(sim);
    simuladorEjecutar(&sim, MONTICULO, 0, 0);
    EstadNiveles off = calcular(sim);
    QApplication::restoreOverrideCursor();

    for (int fila = 0; fila < 6; fila++) {
        int n = (fila < 5) ? fila + 1 : 0;   // las filas 0 a 4 son los niveles 1 a 5; la 5 es el global
        tblAging->setItem(fila, 0, celda(fila < 5 ? QString::number(n) : QString("Global")));
        tblAging->setItem(fila, 1, celda(QString::number(on.cant[n])));
        tblAging->setItem(fila, 2, celda(QString::number(promedio(on, n), 'f', 2)));
        tblAging->setItem(fila, 3, celda(QString::number(on.maxi[n])));
        tblAging->setItem(fila, 4, celda(QString::number(promedio(off, n), 'f', 2)));
        tblAging->setItem(fila, 5, celda(QString::number(off.maxi[n])));
    }

    QStringList cats;
    QVector<double> serieOn, serieOff;
    for (int n = 1; n <= NIVELES; n++) {
        cats << QString("Nivel %1").arg(n);
        serieOn.append(promedio(on, n));
        serieOff.append(promedio(off, n));
    }
    grafAging->establecer(cats, serieOn, serieOff, "Aging activado", "Aging desactivado",
                          "Espera promedio por nivel de urgencia original (min)");

    lblAging->setText(QString("Con aging: %1 ascensos, espera máxima %2 min, tiempo total %3 min.   "
                              "Sin aging: espera máxima %4 min, tiempo total %5 min.")
                          .arg(on.ascensos)
                          .arg(on.maxi[0])
                          .arg(on.tiempoTotal)
                          .arg(off.maxi[0])
                          .arg(off.tiempoTotal));
}

// ======================= Pestana Comparacion =======================

void VentanaPrincipal::alCompararEstructuras() {
    const int tamanos[4] = {1000, 5000, 10000, 20000};
    double razon = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    for (int k = 0; k < 4; k++) {
        int n = tamanos[k];
        Paciente* lote = new Paciente[n];
        std::srand(42);   // mismo lote siempre
        for (int i = 0; i < n; i++) {
            pacienteIniciar(&lote[i], i + 1, "P", 1 + std::rand() % 5, i + 1, 10, 1);
        }
        double hIns, hExt, lIns, lExt;
        long hComp, lComp;
        medirCola(MONTICULO, lote, n, &hIns, &hExt, &hComp);
        medirCola(LISTA, lote, n, &lIns, &lExt, &lComp);
        delete[] lote;

        tblComparacion->setItem(k, 0, celda(QString::number(n)));
        tblComparacion->setItem(k, 1, celda(QString::number(hIns, 'f', 3)));
        tblComparacion->setItem(k, 2, celda(QString::number(lIns, 'f', 3)));
        tblComparacion->setItem(k, 3, celda(QString::number(hExt, 'f', 3)));
        tblComparacion->setItem(k, 4, celda(QString::number(lExt, 'f', 3)));
        tblComparacion->setItem(k, 5, celda(QString::number(hComp)));
        tblComparacion->setItem(k, 6, celda(QString::number(lComp)));
        if (hComp > 0) razon = double(lComp) / hComp;
    }
    QApplication::restoreOverrideCursor();

    lblComparacion->setText(QString("Con N = 20 000, la lista hizo %1 veces las comparaciones del montículo. "
                                    "Los tiempos cambian con cada equipo y ejecución; las comparaciones no.")
                                .arg(razon, 0, 'f', 0));
}