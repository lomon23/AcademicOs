#ifndef TODORIGHTBAR_H
#define TODORIGHTBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include "src/modules/todo/core/ToDoModule.h"

class ToDoRightBar : public QWidget {
    Q_OBJECT

public:
    explicit ToDoRightBar(QWidget *parent = nullptr);

    // Метод, який приймає модуль, рахує статистику і малює графіки
    void refreshStats(ToDoModule *module);

private:
    QVBoxLayout *contentLayout;
    
    // Допоміжний метод для створення одного прогрес-бару
    QWidget* createProgressItem(const QString &name, const QString &color, int done, int total);
};

#endif // TODORIGHTBAR_H