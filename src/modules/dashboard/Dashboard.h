#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <QLineEdit>
#include <QCompleter> // Для автопідказки

#include "src/modules/todo/core/ToDoModule.h"

class Dashboard :public QWidget {
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    void addModuleWidget(QWidget* widget);
    void setToDoModule(ToDoModule *module);
signals:
    void widgetAdded();
    void requestWidget(const QString &widgetType);
    void requestDailyPage(); // <--- НОВИЙ СИГНАЛ
    void navigationRequested(const QString &pageId);
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
    ToDoModule *todoModule = nullptr;
    void updateToDoWidget();
    void setupUi();
    void setupSearch(); // Налаштування автодоповнення
};


#endif