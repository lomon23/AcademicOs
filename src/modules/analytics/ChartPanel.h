#ifndef CHARTPANEL_H
#define CHARTPANEL_H

#include <QWidget>
#include "../../../libs/qcustomplot/qcustomplot.h" // Перевір шлях до бібліотеки!

class ChartPanel : public QWidget {
    Q_OBJECT
public:
    explicit ChartPanel(QWidget *parent = nullptr);
    
    // Метод, щоб завантажити дані з бекенду і намалювати
    void updateChart(const QString &metricId = "");

private:
    QCustomPlot *customPlot; // Сам віджет графіка
    void setupDarkTheme();   // Винесемо налаштування краси в окрему функцію
};

#endif // CHARTPANEL_H