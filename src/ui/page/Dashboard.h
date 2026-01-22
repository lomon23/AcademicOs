#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QLineEdit>
#include <QCompleter> // Для автопідказки
#include "../../modules/analytics/AnalyticsSmallWidget.h"

class Dashboard :public QWidget {
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    void addModuleWidget(QWidget* widget);
signals:
    void widgetAdded();
    void requestWidget(const QString &widgetType);
    void requestDailyPage(); // <--- НОВИЙ СИГНАЛ
private slots:
    void onAddClicked();
    void onSearchReturnPressed();
private:
    // Layouts
    QVBoxLayout *mainLayout;    // Головний вертикальний
    QWidget *searchContainer;   // Контейнер для поля пошуку (щоб ховати/показувати)
    QGridLayout *gridLayout;    // Сітка для віджетів

    // UI Elements
    QLineEdit *searchBar;
    QCompleter *completer;

    // State
    QVector<QWidget*> widgets;
    
    void setupUi();
    void setupSearch(); // Налаштування автодоповнення
};


#endif