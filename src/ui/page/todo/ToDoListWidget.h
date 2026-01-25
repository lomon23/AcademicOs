#ifndef TODOLISTWIDGET_H
#define TODOLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "../../../core/todo/ToDoModule.h"
#include "TaskItemWidget.h" // Включаємо атом

class ToDoListWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToDoListWidget(const ToDoCategory &cat, QWidget *parent = nullptr);
    
    void addTaskWidget(TaskItemWidget *widget); // Додати готову таску
    void clear(); // Очистити перед рендером
    QString getCategoryId() const { return m_cat.id; }

signals:
    void addTaskRequested(QString catId); // Клік по "+" біля назви списку

private:
    ToDoCategory m_cat;
    bool isExpanded = true;

    // UI
    QVBoxLayout *tasksLayout;
    QWidget *tasksContainer;
    QPushButton *toggleBtn;
    QPushButton *addBtn;
};

#endif // TODOLISTWIDGET_H