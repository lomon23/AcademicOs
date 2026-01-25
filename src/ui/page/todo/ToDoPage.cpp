#include "ToDoPage.h"
#include <QLabel>
#include <QInputDialog>
#include <algorithm> 
#include <QTimer>
#include <QFile>
#include <QDebug>

// --- КОНСТРУКТОР ---
ToDoPage::ToDoPage(QWidget *parent) : QWidget(parent) {
    // 1. Спочатку створюємо модуль даних
    module = new ToDoModule(this);
    module->load(); 

    // 2. Будуємо Інтерфейс (вже з модулем)
    setupUI();
    
    // 3. Вантажимо стилі
    loadStyles();

    // 4. Перший рендер (щоб не було пустого екрану)
    currentTabId = "all"; 
    refreshData(); 
}

// --- SETUP UI (Виправлений) ---
void ToDoPage::setupUI() {
    // 1. Root Layout (Весь екран: Ліва частина + Сайдбар)
    QHBoxLayout *rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0); // Склеюємо Main і RightBar (Strict Style)

    // === ЛІВА ЧАСТИНА (MAIN CONTAINER) ===
    QWidget *mainContainer = new QWidget(this);
    mainContainer->setObjectName("PageBackground"); // Чорний фон
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    
    // 🔥 НІЯКИХ ВІДСТУПІВ - Хедер на всю ширину
    mainLayout->setContentsMargins(0, 0, 0, 0); 
    mainLayout->setSpacing(0); // 🔥 СКЛЕЮЄМО Хедер, Таби і Контент

    // --- БЛОК 1: HEADER (Заголовок "My Tasks") ---
    QWidget *headerBlock = new QWidget(this);
    headerBlock->setObjectName("HeaderBlock");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerBlock);
    
    // 🔥 ЗМІНА ТУТ: (Зліва, Зверху, Справа, Знизу)
    // 60px зверху — це те, що робить дизайн "чистим"
    headerLayout->setContentsMargins(40, 60, 40, 10); 
    
    QLabel *title = new QLabel(tr("My Tasks"), this);
    title->setObjectName("PageTitle");
    headerLayout->addWidget(title);
    
    mainLayout->addWidget(headerBlock);

    // --- БЛОК 2: TABS (Приклеєні до Хедера) ---
    QWidget *tabsWidget = new QWidget(this);
    tabsWidget->setObjectName("TabsBlock");
    tabsLayout = new QHBoxLayout(tabsWidget);
    tabsLayout->setContentsMargins(40, 0, 40, 0); // Вирівняно по хедеру
    tabsLayout->setSpacing(5); 
    tabsLayout->setAlignment(Qt::AlignLeft);
    
    mainLayout->addWidget(tabsWidget);

    // --- БЛОК 3: CONTENT (Список завдань) ---
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setObjectName("ContentBlock");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    
    // 🔥 ГОЛОВНИЙ ФІКС БІЛОГО ФОНУ
    // Робимо скролл прозорим, щоб просвічувався наш темний фон
    scroll->setStyleSheet("background: transparent; border: none;"); 
    scroll->viewport()->setStyleSheet("background: transparent;"); 

    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("TransparentContent");
    
    // 🔥 ОСЬ ТУТ БУЛА ПОМИЛКА: Ми створюємо Layout ПЕРЕД використанням
    contentLayout = new QVBoxLayout(scrollContent); 
    
    // Налаштування контенту
    contentLayout->setContentsMargins(30, 20, 30, 20); // Відступи, щоб таски не липли до країв
    contentLayout->setSpacing(15);
    contentLayout->setAlignment(Qt::AlignTop); 

    scroll->setWidget(scrollContent);
    mainLayout->addWidget(scroll);

    // Додаємо ліву частину в головний
    rootLayout->addWidget(mainContainer, 1);

    // === ПРАВА ЧАСТИНА (RIGHT BAR) ===
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

    QString tabStyle = "QPushButton { color: #888; border: none; font-weight: bold; font-size: 14px; padding: 5px; margin-right: 10px; }"
                       "QPushButton:checked { color: white; border-bottom: 2px solid #00E676; }";

    auto createBtn = [&](QString text, QString id) {
        QPushButton *btn = new QPushButton(text, this);
        btn->setObjectName("TabButton");
        btn->setCheckable(true);
        btn->setChecked(currentTabId == id);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(tabStyle);
        connect(btn, &QPushButton::clicked, [this, id](){ 
            currentTabId = id; // Оновлюємо ID
            refreshData();     // Перемальовуємо
        });
        tabsLayout->addWidget(btn);
    };

    createBtn(tr("All"), "all");
    
    for (const auto &cat : module->getCategories()) {
        createBtn(cat.name, cat.id);
    }

    // New List Button (+)
    QPushButton *newBtn = new QPushButton("+", this);
    newBtn->setFixedSize(24, 24);
    newBtn->setCursor(Qt::PointingHandCursor);
    newBtn->setStyleSheet("background: #333; color: white; border-radius: 4px; border: none;");
    
    // 🔥 ЛОГІКА СТВОРЕННЯ СПИСКУ ТУТ
    connect(newBtn, &QPushButton::clicked, [this](){
        module->addCategory("New List", "#BD93F9"); // Створити
        
        auto cats = module->getCategories();
        if (!cats.isEmpty()) {
            currentTabId = cats.last().id; // Переключитись на новий
            refreshData(); 
        }
    });
    
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
            // 🔥 ВИПРАВЛЕНО: Додали 'module' першим аргументом
            ToDoListWidget *lw = new ToDoListWidget(module, cat, this);
            
            connect(lw, &ToDoListWidget::addTaskRequested, [this](QString cid){
                if (currentTabId != cid) currentTabId = cid; 
                taskToFocusId = module->addTask("", cid, ""); 
                refreshData(); 
            });
            
            // В режимі ALL нам не обов'язково слухати перейменування категорій,
            // але можна додати, якщо хочеш.

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
            // Тут у тебе все було правильно
            ToDoListWidget *lw = new ToDoListWidget(module, currentCat, this);
            
            connect(lw, &ToDoListWidget::categoryChanged, this, [this](){
                refreshData(); 
            });

            connect(lw, &ToDoListWidget::deleteCategoryRequested, this, [this](QString catId){
                module->deleteCategory(catId);
                currentTabId = "all"; 
                refreshData();
            });
            
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
void ToDoPage::loadStyles() {
    // Шлях відносно папки build (де лежить .exe)
    QFile file("resources/todo.qss"); 
    
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Warning: Could not load styles/todo.qss";
    }
}