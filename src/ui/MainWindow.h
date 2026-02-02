#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QMap>

// UI Components
#include "components/Sidebar.h"
#include "page/Dashboard.h"
#include "page/DailyPage.h"
// Core & Modules
#include "../modules/Module.h" // Базовий клас модуля
#include "../core/todo/ToDoModule.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleWidgetCreation(const QString &widgetName);
    void openDailyPage();

private:
    // --- UI Methods ---
    void setupUI();
    void setupModules();
    void setupConnections();
    
    // --- Logic Methods ---
    void registerPage(const QString &id, QWidget *page);
    void createFinance(); // Динамічне створення фінансів

    // --- State Management ---
    void saveDashboard();
    void loadDashboard();

    // --- Components ---
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    Sidebar *sidebar;
    QStackedWidget *pagesStack;
    QMap<QString, int> pageMap;

    // --- Modules & Pages ---
    Dashboard *dashboardPage;
    ToDoModule *todoModule; // Зберігаємо вказівник, бо він потрібен дашборду
    DailyPage *dailyPage;
    QList<QObject*> activeModules; // Список всіх активних модулів
};

#endif // MAINWINDOW_H