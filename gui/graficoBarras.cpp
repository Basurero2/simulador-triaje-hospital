#include "GraficoBarras.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QtGlobal>

GraficoBarras::GraficoBarras(QWidget* padre) : QWidget(padre) {
    setMinimumHeight(240);
}

void GraficoBarras::establecer(const QStringList& cats, const QVector<double>& serieA,
                               const QVector<double>& serieB, const QString& nomA,
                               const QString& nomB, const QString& tit) {
    categorias = cats;
    a = serieA;
    b = serieB;
    nombreA = nomA;
    nombreB = nomB;
    titulo = tit;
    update();   // pide redibujar
}

void GraficoBarras::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), palette().window());
    p.setPen(palette().windowText().color());
    p.drawText(QRect(0, 6, width(), 20), Qt::AlignHCenter, titulo);
    if (categorias.isEmpty()) return;

    const int izq = 60, der = 15, sup = 55, inf = 40;
    const QRect area(izq, sup, width() - izq - der, height() - sup - inf);

    double maximo = 1.0;
    for (int i = 0; i < a.size(); i++) maximo = qMax(maximo, a[i]);
    for (int i = 0; i < b.size(); i++) maximo = qMax(maximo, b[i]);

    // Ejes y valores extremos
    p.drawLine(area.bottomLeft(), area.bottomRight());
    p.drawLine(area.bottomLeft(), area.topLeft());
    p.drawText(QRect(0, area.top() - 8, izq - 6, 16), Qt::AlignRight, QString::number(qRound(maximo)));
    p.drawText(QRect(0, area.bottom() - 8, izq - 6, 16), Qt::AlignRight, QString("0"));

    // Leyenda
    const QColor colorA(31, 78, 121);
    const QColor colorB(192, 80, 77);
    p.setBrush(colorA);
    p.drawRect(izq, 30, 12, 12);
    p.drawText(izq + 18, 41, nombreA);
    p.setBrush(colorB);
    p.drawRect(izq + 190, 30, 12, 12);
    p.drawText(izq + 208, 41, nombreB);

    // Barras
    const int n = categorias.size();
    const double ancho = double(area.width()) / n;
    const double barra = ancho * 0.38;
    for (int i = 0; i < n; i++) {
        const double x0 = area.left() + i * ancho + ancho * 0.1;
        const double hA = area.height() * (a.value(i) / maximo);
        const double hB = area.height() * (b.value(i) / maximo);
        p.setBrush(colorA);
        p.drawRect(QRectF(x0, area.bottom() - hA, barra, hA));
        p.setBrush(colorB);
        p.drawRect(QRectF(x0 + barra, area.bottom() - hB, barra, hB));
        p.drawText(QRectF(area.left() + i * ancho, area.bottom() + 4, ancho, 30),
                   Qt::AlignHCenter | Qt::TextWordWrap, categorias[i]);
    }
}