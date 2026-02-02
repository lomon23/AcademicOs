#ifndef CHARTPANEL_H
#define CHARTPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include "../../libs/qcustomplot/qcustomplot.h"

// Допоміжний клас для кольорів (залишаємо як було)
class NeonPalette {
public:
    static QColor getColor(int index) {
        static const QVector<QString> colors = {
            "#50FA7B", "#FF79C6", "#8BE9FD", "#BD93F9", "#FFB86C", "#F1FA8C", "#FF5555"
        };
        return QColor(colors[index % colors.size()]);
    }
};

class ChartPanel : public QWidget {
    Q_OBJECT
public:
    explicit ChartPanel(QWidget *parent = nullptr);

public slots:
    void updateChart(const QStringList &metricIds = QStringList());

private slots:
    // Слоти для навігації
    void onPrevClicked();
    void onNextClicked();
    void onModeChanged(int mode); // 0=Week, 1=Month, 2=Year

private:
    QCustomPlot *customPlot;
    
    // UI Елементи тулбару
    QLabel *dateLabel;
    QPushButton *btnWeek;
    QPushButton *btnMonth;
    QPushButton *btnYear;

    // Стан (State)
    enum TimeMode { Week, Month, Year };
    TimeMode currentMode;
    QDate currentDate; // Дата, на яку ми дивимось (початок періоду або центр)
    QStringList currentMetricIds; // Запам'ятовуємо, що ми зараз малюємо

    void setupDarkTheme();
    void setupToolbar(QVBoxLayout *layout);
    void updateDateLabel();
    void updateButtonsStyle(); // Підсвітити активну кнопку (Week/Month/Year)
    
    // Хелпери для розрахунку початку і кінця періоду
    QPair<QDateTime, QDateTime> getCurrentRange() const;
};

#endif // CHARTPANEL_H