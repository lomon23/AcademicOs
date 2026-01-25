#ifndef TODOPAGE_H
#define TODOPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QMap>

#include "../../../core/todo/ToDoModule.h"
#include "ToDoRightBar.h"
#include "TaskItemWidget.h"
#include "ToDoListWidget.h"

class ToDoPage : public QWidget {
    Q_OBJECT

public:
    explicit ToDoPage(QWidget *parent = nullptr);
    void refreshData();
    void setModule(ToDoModule *m) { module = m; }

private slots:
    void onTabSwitch(QString id);

    void onAddNewList();

private:
    ToDoModule *module = nullptr;
    QString currentTabId = "all";
    QString taskToFocusId = ""; // ID нової таски для автофокусу

    // UI Elements
    QHBoxLayout *tabsLayout;
    QVBoxLayout *contentLayout;

    ToDoRightBar *rightBar;

    // Render Logic
    void setupUI();
    void renderTabs();
    void renderContent();
    void clearLayout(QLayout *layout);
};

#endif // TODOPAGE_H