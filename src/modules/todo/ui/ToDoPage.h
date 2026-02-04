#ifndef TODOPAGE_H
#define TODOPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QPushButton>
#include <QMap>

#include "src/modules/todo/core/ToDoModule.h"
#include "src/modules/todo/ui/ToDoRightBar.h"
#include "src/modules/todo/widgets/TaskItemWidget.h"
#include "src/modules/todo/ui/ToDoListWidget.h"

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
    ToDoRightBar *rightBar;          // <--- Має бути тут

    
    ToDoModule *module = nullptr;
    QString currentTabId = "all";
    QString taskToFocusId = "";

    // UI Elements
    QHBoxLayout *tabsLayout;
    QVBoxLayout *contentLayout;


    // Render Logic
    void setupUI();
    void renderTabs();
    void renderContent();
    void clearLayout(QLayout *layout);
    void loadStyles();
};

#endif // TODOPAGE_H