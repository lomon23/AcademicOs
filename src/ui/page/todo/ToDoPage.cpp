#include "ToDoPage.h"
#include <QLabel>
#include <QInputDialog>
#include <algorithm> // для std::reverse
#include <QTimer>

ToDoPage::ToDoPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ToDoPage::setupUI() {
    // 1. Root Layout
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // === LEFT SIDE ===
    QWidget *mainContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Header: Title + Tabs
    QLabel *title = new QLabel(tr("My Tasks"), this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);

    QWidget *tabsWidget = new QWidget(this);
    tabsLayout = new QHBoxLayout(tabsWidget);
    tabsLayout->setContentsMargins(0, 0, 0, 0);
    tabsLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(tabsWidget);

    // Scroll Area
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent; border: none;");

    QWidget *scrollContent = new QWidget();
    contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);
    contentLayout->setAlignment(Qt::AlignTop); 

    scroll->setWidget(scrollContent);
    mainLayout->addWidget(scroll);

    // ❌ ТУТ БУВ FOOTER (quickInput). МИ ЙОГО ВИДАЛИЛИ.

    rootLayout->addWidget(mainContainer, 1);

    // === RIGHT SIDE ===
    rightBar = new ToDoRightBar(this);
    rootLayout->addWidget(rightBar);
}

void ToDoPage::refreshData() {
    if (!module) return;
    renderTabs();
    renderContent();
    rightBar->refreshStats(module);
}

void ToDoPage::renderTabs() {
    clearLayout(tabsLayout);

    // Style for tabs
    QString tabStyle = "QPushButton { color: #888; border: none; font-weight: bold; font-size: 14px; padding: 5px; margin-right: 10px; }"
                       "QPushButton:checked { color: white; border-bottom: 2px solid #00E676; }";

    auto createBtn = [&](QString text, QString id) {
        QPushButton *btn = new QPushButton(text, this);
        btn->setCheckable(true);
        btn->setChecked(currentTabId == id);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(tabStyle);
        connect(btn, &QPushButton::clicked, [this, id](){ onTabSwitch(id); });
        tabsLayout->addWidget(btn);
    };

    createBtn(tr("All"), "all");
    
    for (const auto &cat : module->getCategories()) {
        createBtn(cat.name, cat.id);
    }

    // New List Button
    QPushButton *newBtn = new QPushButton("+", this);
    newBtn->setFixedSize(24, 24);
    newBtn->setCursor(Qt::PointingHandCursor);
    newBtn->setStyleSheet("background: #333; color: white; border-radius: 4px; border: none;");
    connect(newBtn, &QPushButton::clicked, this, &ToDoPage::onAddNewList);
    tabsLayout->addWidget(newBtn);
}

void ToDoPage::renderContent() {
    clearLayout(contentLayout);
    if (!module) return;

    // 1. ОТРИМАННЯ ДАНИХ
    QVector<ToDoTask> tasks;
    if (currentTabId == "all") {
        for (const auto &cat : module->getCategories()) {
            tasks.append(module->getTasksByCategory(cat.id));
        }
    } else {
        tasks = module->getTasksByCategory(currentTabId);
    }

    // 2. СТВОРЕННЯ КОНТЕЙНЕРІВ (Lists)
    QMap<QString, TaskItemWidget*> taskWidgets;
    QMap<QString, ToDoListWidget*> listWidgets;

    // A. РЕЖИМ ALL
    if (currentTabId == "all") {
        for (const auto &cat : module->getCategories()) {
            ToDoListWidget *lw = new ToDoListWidget(cat, this);
            
            // 🔥 МАГІЯ INLINE ADDITION
            connect(lw, &ToDoListWidget::addTaskRequested, [this](QString cid){
                if (currentTabId != cid) currentTabId = cid; // Перемикання
                
                // Створюємо пусту таску і кажемо "ФОКУСУЙСЯ НА НІЙ"
                taskToFocusId = module->addTask("", cid, ""); 
                refreshData(); 
            });

            contentLayout->addWidget(lw);
            listWidgets[cat.id] = lw;
        }
    } 
    // B. РЕЖИМ TAB
    else {
        ToDoCategory currentCat;
        bool found = false;
        for (const auto &c : module->getCategories()) {
            if (c.id == currentTabId) { currentCat = c; found = true; break; }
        }

        if (found) {
            ToDoListWidget *lw = new ToDoListWidget(currentCat, this);
            
            // 🔥 ТА Ж САМА МАГІЯ ТУТ
            connect(lw, &ToDoListWidget::addTaskRequested, [this](QString cid){
                taskToFocusId = module->addTask("", cid, ""); 
                refreshData();
            });
            
            contentLayout->addWidget(lw);
            listWidgets[currentCat.id] = lw;
        }
    }

    // 3. СТВОРЕННЯ ТАСОК
    for (const auto &task : tasks) {
        TaskItemWidget *w = new TaskItemWidget(task, this);
        
        connect(w, &TaskItemWidget::statusChanged, this, [this](QString id, bool done){
            module->toggleTask(id); refreshData();
        }, Qt::QueuedConnection);

        connect(w, &TaskItemWidget::textChanged, [this](QString id, QString txt){
            module->renameTask(id, txt);
        });

        connect(w, &TaskItemWidget::deleteRequested, this, [this](QString id){
            module->deleteTask(id); refreshData();
        }, Qt::QueuedConnection);

        connect(w, &TaskItemWidget::addSubTaskRequested, this, [this, task](QString pid){
             taskToFocusId = module->addTask("", task.categoryId, pid);
             refreshData();
        }, Qt::QueuedConnection);


        // 👇 ПІДКЛЮЧАЄМО ПРІОРИТЕТ
        connect(w, &TaskItemWidget::priorityChangeRequested, this, [this](QString id){
            module->cyclePriority(id); // Міняємо в базі (0->1->2...)
            refreshData();             // Перемальовуємо (щоб колір оновився)
        }, Qt::QueuedConnection);

        // 👇 АВТОФОКУС: Якщо це та таска, яку ми щойно створили
        if (task.id == taskToFocusId) {
            // Використовуємо Timer, щоб віджет встиг з'явитись на екрані перед тим, як ми дамо йому фокус
            QTimer::singleShot(0, w, &TaskItemWidget::startEditing);
            taskToFocusId = ""; 
        }

        taskWidgets[task.id] = w;
    }

    // 4. ЛІНКУВАННЯ
    for (const auto &task : tasks) {
        if (!taskWidgets.contains(task.id)) continue;
        TaskItemWidget *widget = taskWidgets[task.id];
        QString pid = task.parentTaskId;

        if (!pid.isEmpty() && taskWidgets.contains(pid)) {
            widget->setIndentLevel(1); 
        } 
        
        if (listWidgets.contains(task.categoryId)) {
            listWidgets[task.categoryId]->addTaskWidget(widget);
        }
    }
    
    contentLayout->addStretch();
}

void ToDoPage::onTabSwitch(QString id) {
    if (currentTabId == id) return;
    currentTabId = id;
    refreshData();
}


void ToDoPage::onAddNewList() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("New List"), tr("Name:"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty() && module) {
        module->addCategory(name, "#FFFFFF"); // Randomize color later
        refreshData();
    }
}

void ToDoPage::clearLayout(QLayout *layout) {
    if (!layout) return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0))) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}