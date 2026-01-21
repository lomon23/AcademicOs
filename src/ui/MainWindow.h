#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QList> 
#include <QJsonObject>
#include <QJsonArray>

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
    void handleWidgetCreation(const QString &widgetName);

private:
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    Sidebar *sidebar;
    QStackedWidget *pagesStack;
    Dashboard *dashboardPage;

    // Список всіх активних модулів (Аналітика, Фінанси...)
    QList<QObject*> activeModules; 

    // --- ФАБРИЧНІ МЕТОДИ ---
    void createAnalytics(const QString &title = "New Chart");
    void createFinance();

    // --- ПАМ'ЯТЬ (JSON) ---
    void saveDashboard(); // Зберігає список віджетів
    void loadDashboard(); // Відновлює їх   
};

#endif // MAINWINDOW_H