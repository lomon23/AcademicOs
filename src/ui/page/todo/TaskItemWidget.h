#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "../../../core/todo/ToDoModule.h" // Перевір шлях до ToDoModule

class TaskItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit TaskItemWidget(const ToDoTask &task, QWidget *parent = nullptr);
    
    void setIndentLevel(int level); // Щоб робити відступ для підзадач
    void startEditing();            // Вмикає режим редагування (для нових тасок)
    QString getTaskId() const { return m_task.id; }
    QString getParentId() const { return m_task.parentTaskId; }

signals:
    void statusChanged(QString id, bool isDone);
    void textChanged(QString id, QString newTitle);
    void deleteRequested(QString id);
    void addSubTaskRequested(QString parentId);

private:
    ToDoTask m_task;
    
    // UI Elements
    QCheckBox *checkBox;
    QLabel *titleLabel;
    QLineEdit *titleEdit;
    QPushButton *addSubTaskBtn;
    QPushButton *deleteBtn;
    QHBoxLayout *mainLayout;
};

#endif // TASKITEMWIDGET_H