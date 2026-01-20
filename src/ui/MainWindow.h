#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QStackedWidget>
#include <QList>

// Підключаємо твої класи
#include "components/Sidebar.h"
#include "page/Dashboard.h" // Перевір шлях (page чи pages?)
#include "../modules/analytics/AnalyticsModule.h" // <--- Для AnalyticsModule

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddWidgetClicked(); // Слот для додавання

private:
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    
    // --- ВАЖЛИВО: Оголошуємо змінні тут ---
    Sidebar *sidebar;               // <--- Оголосили
    QStackedWidget *pagesStack;     // <--- Оголосили
    Dashboard *dashboardPage;       // <--- Оголосили
    
    // Список модулів
    QList<AnalyticsModule*> activeModules;
};

#endif // MAINWINDOW_H