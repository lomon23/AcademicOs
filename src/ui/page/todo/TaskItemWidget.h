#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit> // <-- Додали
#include "../../../core/todo/Task.h"

class TaskItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit TaskItemWidget(const ToDoTask &task, QWidget *parent = nullptr);
    void updateData(const ToDoTask &task);
    QString getTaskId() const { return taskId; }

    void addChildTask(QWidget* childWidget);

signals:
    void statusChanged(QString taskId, bool isDone);
    void deleteRequested(QString taskId);
    void addSubTaskRequested(QString parentTaskId);
    
    // 👇 Сигнал про зміну назви
    void renameRequested(QString taskId, QString newTitle); 

private slots:
    void onCheckboxClicked(int state);
    void onEditClicked(); // Клік на олівець
    void onSaveClicked(); // Збереження редагування

private:
    QString taskId;
    QCheckBox *checkBox;
    
    // UI для перегляду
    QLabel *titleLabel;
    
    // UI для редагування
    QLineEdit *titleEdit;
    
    // Кнопки
    QPushButton *editBtn;
    QPushButton *deleteBtn;
    QPushButton *addSubTaskBtn;

    QVBoxLayout *mainLayout; 
    QWidget *childrenContainer; 
    QVBoxLayout *childrenLayout;

    bool isEditMode = false; // Стан віджета

    void updateTextStyle(bool isDone);
    void toggleEditMode(bool enable); // Перемикач
};

#endif // TASKITEMWIDGET_H