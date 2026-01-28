#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include <QWidget>
#include <QVBoxLayout>

class MetricsPanel : public QWidget {
    Q_OBJECT
public:
    explicit MetricsPanel(QWidget *parent = nullptr);
    
    // Головний метод: каже панелі "Покажи метрики для цієї категорії"
    void setCategory(const QString &category);

signals:
    void dataChanged();
    void metricSelected(QString id);

private slots:
    void onAddMetricClicked();
    void onGenDataClicked(); // Залишимо кнопку для тестів

private:
    QVBoxLayout *contentLayout;
    QString currentCategory; // Зберігаємо поточну категорію (Health, Code...)
};

#endif // METRICSPANEL_H