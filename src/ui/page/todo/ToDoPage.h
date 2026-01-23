#ifndef TODOPAGE_H
#define TODOPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout> // Додаємо горизонтальний лейаут
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QPushButton>

#include "../../../core/todo/ToDoModule.h"
#include "CategoryWidget.h"
#include "ToDoRightBar.h" // <-- Підключаємо наш новий віджет

class ToDoPage : public QWidget {
    Q_OBJECT

public:
    explicit ToDoPage(QWidget *parent = nullptr);
    void refreshData();
    void setModule(ToDoModule *m) { todoModule = m; } 

private slots:
    void onAddTaskClicked();
    void onTaskStatusChanged(QString taskId, bool isDone);
    void onAddCategoryClicked(); // Залишаємо (якщо є) або видаляємо

private:
    ToDoModule *todoModule = nullptr;
    ToDoModule* getModule();

    // UI Elements
    QLineEdit *taskInput;
    QComboBox *categoryCombo;
    QVBoxLayout *categoriesLayout;
    
    // 👇 НОВЕ: Правий сайдбар
    ToDoRightBar *rightBar; 
    
    // 👇 НОВЕ: Кнопка вибору кольору та поточний індекс кольору
    QPushButton *colorSelectorBtn;
    int currentColorIndex = 0;
    const QStringList categoryColors = {
        "#FF5733", // Red
        "#33FF57", // Green
        "#3357FF", // Blue
        "#F033FF", // Purple
        "#FFFF33", // Yellow
        "#00E676"  // Teal
    };

    void setupUI();
    void clearLayout();
    
    // Метод перемикання кольору
    void cycleColor();
};

#endif // TODOPAGE_H