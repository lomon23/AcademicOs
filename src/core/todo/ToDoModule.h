#ifndef TODOMODULE_H
#define TODOMODULE_H

#include "../../modules/Module.h" // Шлях до твого Module.h (перевір рівень вкладеності)
#include "Task.h"
#include <QVector>
#include <QObject>
#include <QWidget>

class ToDoModule : public Module {
    Q_OBJECT

public:
    explicit ToDoModule(QObject *parent = nullptr);

    // --- Методи з Module.h (override ОБОВ'ЯЗКОВИЙ) ---
    void save() override;
    void load() override;

    // --- Власні методи (override НЕ ПОТРІБЕН) ---
    QWidget* createFullPage();
    QWidget* createSmallWidget();

    // --- CRUD ---
    void addCategory(const QString& name, const QString& color);
    void addTask(const QString& title, const QString& categoryId, const QString& parentTaskId = "");
    void toggleTask(const QString& taskId);
    void deleteTask(const QString& taskId);
    

    void renameTask(const QString& taskId, const QString& newTitle);
    
    // Геттери для UI
    QVector<ToDoCategory> getCategories() const { return categories; }
    QVector<ToDoTask> getTasksByCategory(const QString& catId) const;

private:
    QVector<ToDoCategory> categories;
    QVector<ToDoTask> tasks;

    const QString STORAGE_KEY = "todo_data";
};

#endif // TODOMODULE_H