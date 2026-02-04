#ifndef TODOSMALLWIDGET_H
#define TODOSMALLWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "src/modules/todo/core/ToDoModule.h"

class ToDoSmallWidget : public QWidget {
    Q_OBJECT

public:
    explicit ToDoSmallWidget(ToDoModule *module, QWidget *parent = nullptr);
    
    // Метод оновлення (викликається автоматично при зміні даних)
    void refresh();

signals:
    void clicked(); // Сигнал, щоб Дашборд знав, коли ми клікнули (щоб відкрити ToDo Page)

protected:
    // Перехоплюємо клік мишкою
    void mousePressEvent(QMouseEvent *event) override;

private:
    ToDoModule *module;
    QVBoxLayout *contentLayout; // Сюди кладемо рядки
    
    QWidget* createCategoryItem(const QString &name, const QString &color, int done, int total);
};

#endif // TODOSMALLWIDGET_H