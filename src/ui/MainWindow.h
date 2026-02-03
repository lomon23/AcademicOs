#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QMap>

// --- UI Components ---
#include "components/Sidebar.h"
#include "page/Dashboard.h"
#include "page/DailyPage.h" // DailyPage оголошений тут

// --- Core & Modules ---
#include "../modules/Module.h" 
#include "../core/todo/ToDoModule.h"
#include "../modules/finance/FinanceFullPage.h" // Додаємо FinanceFullPage

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
    void createFinance(); 

    // --- State Management ---
    void saveDashboard();
    void loadDashboard();

    // --- Components ---
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    Sidebar *sidebar;
    QStackedWidget *pagesStack;
    QMap<QString, int> pageMap;

    // --- Modules & Pages (Члени класу) ---
    Dashboard *dashboardPage = nullptr;
    DailyPage *dailyPage = nullptr;     // <--- ВАЖЛИВО: Оголошений тут
    FinanceFullPage *financePage = nullptr; // <--- ВАЖЛИВО: Оголошений тут
    
    ToDoModule *todoModule = nullptr;
    QList<QObject*> activeModules; 
};

#endif // MAINWINDOW_H