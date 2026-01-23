#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>

// Підключаємо структуру даних з CORE
// (Можливо, шлях буде іншим, наприклад "../../core/todo/Task.h" або просто "core/todo/Task.h")
#include "../../../core/todo/Task.h" 

class TaskItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit TaskItemWidget(const ToDoTask &task, QWidget *parent = nullptr);

    // Метод для оновлення даних без перестворення віджета
    void updateData(const ToDoTask &task);
    
    // Геттер ID, щоб батьківський віджет знав, хто це
    QString getTaskId() const { return taskId; }

signals:
    // Сигнал нагору: "Мене клікнули, ось мій ID і новий стан"
    void statusChanged(QString taskId, bool isDone);

private slots:
    void onCheckboxClicked(int state);

private:
    QString taskId;
    QCheckBox *checkBox;
    QLabel *titleLabel;
    
    // Внутрішній метод для зміни стилю (закреслення тексту)
    void updateTextStyle(bool isDone);
};

#endif // TASKITEMWIDGET_H