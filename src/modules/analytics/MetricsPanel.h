#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include <QWidget>
#include <QVBoxLayout>

class MetricsPanel : public QWidget {
    Q_OBJECT
public:
    explicit MetricsPanel(QWidget *parent = nullptr);
    
    // Метод, щоб перемалювати список (наприклад, після додавання нової)
    void refreshMetrics();


signals:
    void dataChanged();
    void metricSelected(QString id);

private slots:
    void onAddMetricClicked();
    void onGenDataClicked();

private:
    QVBoxLayout *contentLayout; // Сюди будемо додавати віджети метрик
};

#endif // METRICSPANEL_H