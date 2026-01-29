#ifndef METRICSPANEL_H
#define METRICSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStringList> // Важливо для списків
#include <QMenu>
#include <QAction>
#include <QColorDialog>

class MetricsPanel : public QWidget {
    Q_OBJECT
public:
    explicit MetricsPanel(QWidget *parent = nullptr);
    
    // Метод, щоб перемалювати список
    void setCategory(const QString &category);

signals:
    void dataChanged();
    
    // ОНОВЛЕНИЙ СИГНАЛ: Відправляє список ID, а не один ID
    void selectionChanged(QStringList ids); 

private slots:
    void onAddMetricClicked();
    void onGenDataClicked();
    void onContextMenuRequested(const QPoint &pos);

private:
    QVBoxLayout *contentLayout;
    QString currentCategory;
    
    // НОВА ЗМІННА: Список вибраних метрик
    QStringList selectedMetrics; 
};

#endif // METRICSPANEL_H