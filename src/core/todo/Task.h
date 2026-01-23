#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDate>
#include <QUuid>

struct ToDoCategory {
    QString id;
    QString name;
    QString color;
    bool isExpanded;

    ToDoCategory(QString n = "New List", QString c = "#FFFFFF") 
        : id(QUuid::createUuid().toString()), name(n), color(c), isExpanded(true) {}
};

struct ToDoTask {
    QString id;
    QString title;
    bool isDone;
    QString categoryId; 
    QString parentTaskId; // <--- НОВЕ: ID батьківського завдання (для під-списків)
    bool isRecurring;
    QDate createdDate;

    ToDoTask(QString t = "", QString catId = "", QString parentId = "") 
        : id(QUuid::createUuid().toString()), 
          title(t), 
          isDone(false), 
          categoryId(catId), 
          parentTaskId(parentId),
          isRecurring(false),
          createdDate(QDate::currentDate()) {}
};

#endif // TASK_H