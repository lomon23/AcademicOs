#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QMap>

// --- UI Components ---
#include "src/ui/components/sidebar/Sidebar.h"

// --- Forward Declarations Only (Швидша компіляція!) ---
class Dashboard;
class DailyPage;
class FinanceFullPage;
class ToDoModule;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openDailyPage();

private:
    // --- Setup ---
    void setupUI();
    void setupModules();     // Тут ініціалізуємо Core (ToDoModule)
    void setupPages();       // Тут використовуємо Factory
    void setupConnections();
    
    // --- Logic ---
    void registerPage(const QString &id, QWidget *page);

    // --- Components ---
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    Sidebar *sidebar;
    QStackedWidget *pagesStack;
    QMap<QString, int> pageMap;

    // --- Pointers to critical pages (для слотів) ---
    // Ми зберігаємо вказівники, але інклудити їх хедери будемо в .cpp
    Dashboard *dashboardPage = nullptr;
    DailyPage *dailyPage = nullptr;
    FinanceFullPage *financePage = nullptr;
    
    // --- Modules ---
    ToDoModule *todoModule = nullptr;
};

#endif // MAINWINDOW_H