#ifndef TODOMODULE_H
#define TODOMODULE_H

#include "src/modules/Module.h" 
#include <QVector>
#include <QObject>
#include <QWidget>
#include <QDate> // <--- ВАЖЛИВО: Додай це, бо використовуєш QDate

// --- 1. ОГОЛОШУЄМО СТРУКТУРИ ТУТ (ЗВЕРХУ) ---

struct ToDoCategory {
    QString id;
    QString name;
    QString color;
    bool isExpanded = true;
};

struct ToDoTask {
    QString id;
    QString title;
    QString categoryId;
    QString parentTaskId;
    bool isDone;
    QString date;
    
    // Поля для логіки
    bool isRecurring = false;
    QDate createdDate;

    // Пріоритет (0=Gray, 1=Green, 2=Yellow, 3=Red)
    int priority = 0; 
};

// --- 2. ТЕПЕР САМ КЛАС (Він вже бачить структури зверху) ---

class ToDoModule : public Module {
    Q_OBJECT

public:
    explicit ToDoModule(QObject *parent = nullptr);

    // Module overrides
    void save() override;
    void load() override;

    // UI creation
    QWidget* createSmallWidget();
    QWidget* createFullPage();

    // Features
    void cyclePriority(const QString& taskId);

    // CRUD
    void addCategory(const QString& name, const QString& color);
    QString addTask(const QString& title, const QString& categoryId, const QString& parentTaskId = "");
    void toggleTask(const QString& taskId);
    void deleteTask(const QString& taskId);
    void renameTask(const QString& taskId, const QString& newTitle);

    void deleteCategory(const QString& catId);
    void updateCategory(const QString& catId, const QString& newName, const QString& newColor);
    
    // Getters
    QVector<ToDoCategory> getCategories() const { return categories; }
    QVector<ToDoTask> getTasksByCategory(const QString& catId) const;

private:
    QVector<ToDoCategory> categories;
    QVector<ToDoTask> tasks;

    const QString STORAGE_KEY = "todo_data";
};

#endif // TODOMODULE_H