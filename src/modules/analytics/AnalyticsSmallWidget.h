#ifndef ANALYTICSSMALLWIDGET_H
#define ANALYTICSSMALLWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "../../../libs/qcustomplot/qcustomplot.h" // Перевір шлях!

class AnalyticsSmallWidget : public QWidget {
    Q_OBJECT
public:
    explicit AnalyticsSmallWidget(QWidget *parent = nullptr);
    void setTitle(const QString &title); // Щоб змінювати назву ззовні
    void setData(const QVector<double> &data);

signals:
    void clicked(); // Сигнал натискання2

protected:
    void mousePressEvent(QMouseEvent *event) override; // Ловимо клік
private:
    QCustomPlot *chart;
    QLabel *titleLabel;
    void setupChart();
};

#endif // ANALYTICSSMALLWIDGET_H