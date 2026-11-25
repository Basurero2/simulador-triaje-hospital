#pragma once
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVector>

// Barras agrupadas: dos series (A y B) por categoria.
class GraficoBarras : public QWidget {
public:
    explicit GraficoBarras(QWidget* padre = nullptr);
    void establecer(const QStringList& cats, const QVector<double>& serieA,
                    const QVector<double>& serieB, const QString& nomA,
                    const QString& nomB, const QString& tit);
protected:
    void paintEvent(QPaintEvent* evento) override;
private:
    QStringList categorias;
    QVector<double> a, b;
    QString nombreA, nombreB, titulo;
};