#ifndef CATEGORYWIDGET_H
#define CATEGORYWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "../../../core/todo/Task.h" // Структура категорії

class CategoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit CategoryWidget(const ToDoCategory &category, QWidget *parent = nullptr);

    // Метод, щоб додати таск у цю категорію
    void addTaskWidget(QWidget *taskWidget);
    
    // Отримати ID категорії (щоб знати, куди додавати нові таски)
    QString getCategoryId() const { return categoryId; }

    // Очистити список перед перемальовкою
    void clearTasks();

private slots:
    void toggleExpand(); // Згорнути/Розгорнути

private:
    QString categoryId;
    QVBoxLayout *tasksLayout; // Сюди падають таски
    QWidget *tasksContainer;  // Віджет, який ховається/показується
    QPushButton *toggleBtn;   // Кнопка стрілочки
};

#endif // CATEGORYWIDGET_H