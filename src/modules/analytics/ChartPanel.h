#ifndef CHARTPANEL_H
#define CHARTPANEL_H

#include <QWidget>
#include "../../libs/qcustomplot/qcustomplot.h"
#include <QStringList> // Додаємо список

// Хелпер для кольорів (Neon Palette)
class NeonPalette {
public:
    static QColor getColor(int index) {
        static const QVector<QString> colors = {
            "#50FA7B", // Green
            "#FF79C6", // Pink
            "#8BE9FD", // Cyan
            "#BD93F9", // Purple
            "#FFB86C", // Orange
            "#F1FA8C", // Yellow
            "#FF5555"  // Red
        };
        return QColor(colors[index % colors.size()]);
    }
};

class ChartPanel : public QWidget {
    Q_OBJECT
public:
    explicit ChartPanel(QWidget *parent = nullptr);

public slots:
    // ЗМІНА: Тепер приймаємо список ID
    void updateChart(const QStringList &metricIds = QStringList());

private:
    QCustomPlot *customPlot;
    void setupDarkTheme();
};

#endif // CHARTPANEL_H