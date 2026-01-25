#ifndef TODOLISTWIDGET_H
#define TODOLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "../../../core/todo/ToDoModule.h"

class ToDoListWidget : public QWidget {
    Q_OBJECT

public:
    // Додаємо module, щоб можна було зберігати зміни
    explicit ToDoListWidget(ToDoModule* module, const ToDoCategory &category, QWidget *parent = nullptr);

    void addTaskWidget(QWidget *w);

signals:
    void addTaskRequested(QString categoryId);
    void categoryChanged(); // Сигнал, щоб ToDoPage оновив таби
    void deleteCategoryRequested(QString categoryId); // Сигнал на видалення

private:
    ToDoModule* m_module;
    ToDoCategory m_cat;
    
    QVBoxLayout *mainLayout;
    QVBoxLayout *tasksLayout; // Сюди падають таски
    
    // Елементи хедера
    QPushButton *colorBtn;
    QLineEdit *titleEdit;
    
    void setupHeader();
    void cycleColor();
};

#endif // TODOLISTWIDGET_H