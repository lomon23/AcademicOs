#ifndef TODOPAGE_H
#define TODOPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>

// Підключаємо наші компоненти
#include "../../../core/todo/ToDoModule.h"
#include "CategoryWidget.h"

class ToDoPage : public QWidget {
    Q_OBJECT

public:
    explicit ToDoPage(QWidget *parent = nullptr);
    void setModule(ToDoModule *m) { todoModule = m; }
    // Метод, який викликається перед показом (щоб оновити дані)
    void refreshData();

private slots:
    void onAddTaskClicked();
    void onTaskStatusChanged(QString taskId, bool isDone);

private:
    // Посилання на модуль (шукаємо його динамічно)
    ToDoModule* getModule();
    ToDoModule *todoModule = nullptr;
    // UI Елементи
    QLineEdit *taskInput;
    QComboBox *categoryCombo;
    QVBoxLayout *categoriesLayout; // Сюди додаємо віджети категорій
    
    // Допоміжний метод для побудови UI
    void setupUI();
    
    // Метод очистки перед оновленням
    void clearLayout();
};

#endif // TODOPAGE_H