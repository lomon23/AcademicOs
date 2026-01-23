#include "ToDoPage.h"
#include "TaskItemWidget.h"
#include <QPushButton>
#include <QRandomGenerator>
#include <QMap>

ToDoPage::ToDoPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ToDoPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 1. HEADER
    QLabel *title = new QLabel("Tasks", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);

    // --- NEW UI: INLINE CATEGORY CREATION ---
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    // Ввід нової категорії
    QLineEdit *newCatInput = new QLineEdit(this);
    newCatInput->setPlaceholderText("New List Name...");
    newCatInput->setFixedWidth(200);
    newCatInput->setStyleSheet("background: #2D2D2D; color: white; border: 1px solid #444; border-radius: 4px; padding: 6px;");
    topLayout->addWidget(newCatInput);

    QPushButton *addCatBtn = new QPushButton("Add List", this);
    addCatBtn->setCursor(Qt::PointingHandCursor);
    addCatBtn->setStyleSheet("background-color: #444; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold;");
    topLayout->addWidget(addCatBtn);

    // Логіка додавання категорії
    connect(addCatBtn, &QPushButton::clicked, [this, newCatInput](){
        QString text = newCatInput->text().trimmed();
        if (!text.isEmpty()) {
            ToDoModule *mod = getModule();
            if (mod) {
                QStringList colors = {"#FF5733", "#33FF57", "#3357FF", "#F033FF", "#FFFF33", "#33FFFF"};
                QString color = colors.at(QRandomGenerator::global()->bounded(colors.size()));
                mod->addCategory(text, color);
                newCatInput->clear();
                refreshData();
            }
        }
    });

    topLayout->addStretch(); // Розділювач
    mainLayout->addWidget(topBar);

    // --- TASK INPUT AREA ---
    QWidget *inputContainer = new QWidget(this);
    inputContainer->setStyleSheet("background-color: #2D2D2D; border-radius: 8px;");
    QHBoxLayout *inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(10, 10, 10, 10);

    taskInput = new QLineEdit(this);
    taskInput->setPlaceholderText("What needs to be done?");
    taskInput->setStyleSheet("border: none; color: white; font-size: 16px; background: transparent;");
    inputLayout->addWidget(taskInput, 1);

    categoryCombo = new QComboBox(this);
    categoryCombo->setFixedWidth(150);
    categoryCombo->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 4px; padding: 4px;");
    inputLayout->addWidget(categoryCombo);

    QPushButton *addTaskBtn = new QPushButton("+", this);
    addTaskBtn->setFixedSize(32, 32);
    addTaskBtn->setCursor(Qt::PointingHandCursor);
    addTaskBtn->setStyleSheet("background-color: #00E676; color: black; font-weight: bold; border-radius: 16px; border: none;");
    connect(addTaskBtn, &QPushButton::clicked, this, &ToDoPage::onAddTaskClicked);
    connect(taskInput, &QLineEdit::returnPressed, this, &ToDoPage::onAddTaskClicked);
    inputLayout->addWidget(addTaskBtn);

    mainLayout->addWidget(inputContainer);

    // --- SCROLL AREA ---
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    categoriesLayout = new QVBoxLayout(scrollContent);
    categoriesLayout->setContentsMargins(0, 0, 0, 0);
    categoriesLayout->setSpacing(15);
    categoriesLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void ToDoPage::refreshData() {
    ToDoModule *mod = getModule();
    if (!mod) return;

    clearLayout();
    categoryCombo->clear();

    QVector<ToDoCategory> categories = mod->getCategories();
    
    // Карта для збереження віджетів завдань, щоб потім розкласти дітей
    QMap<QString, TaskItemWidget*> taskWidgetsMap;
    // Карта: ID категорії -> віджет категорії
    QMap<QString, CategoryWidget*> categoryWidgetsMap;

    // 1. Створюємо Категорії
    for (const auto &cat : categories) {
        categoryCombo->addItem(cat.name, cat.id);
        CategoryWidget *catWidget = new CategoryWidget(cat, this);
        categoryWidgetsMap[cat.id] = catWidget;
        categoriesLayout->insertWidget(categoriesLayout->count() - 1, catWidget);
    }

    // 2. Створюємо ВСІ віджети завдань (поки що "в повітрі")
    // Спочатку треба отримати всі таски з усіх категорій
    // Але в нас API getTasksByCategory. 
    // Окей, пройдемось по категоріях.
    
    QVector<ToDoTask> allTasks;
    for (const auto &cat : categories) {
        allTasks.append(mod->getTasksByCategory(cat.id));
    }

    // Створюємо віджети і запам'ятовуємо їх
    for (const auto &task : allTasks) {
        // Батьківський віджет поки що this (тимчасово)
        TaskItemWidget *item = new TaskItemWidget(task, this);
        
        connect(item, &TaskItemWidget::statusChanged, this, &ToDoPage::onTaskStatusChanged);
        
        connect(item, &TaskItemWidget::deleteRequested, [this, mod](QString id){
            mod->deleteTask(id);
            refreshData();
        });

        // 👇 ПІДКЛЮЧЕННЯ РЕДАГУВАННЯ
        connect(item, &TaskItemWidget::renameRequested, [mod](QString id, QString newTitle){
            mod->renameTask(id, newTitle);
            // Тут refreshData() НЕ потрібен, бо віджет вже оновив свій текст візуально.
            // Модуль просто тихо збереже це в JSON.
        });

        // Перевірка на subtask button (щоб не ламалось, бо ми його можемо не створити)
        connect(item, &TaskItemWidget::addSubTaskRequested, [this, mod, task](QString parentId){
            mod->addTask("New Sub-task", task.categoryId, parentId);
            refreshData();
        });

        taskWidgetsMap[task.id] = item;
    }

    // 3. Розкладаємо завдання по місцях (Будуємо дерево)
    for (const auto &task : allTasks) {
        TaskItemWidget *item = taskWidgetsMap[task.id];
        
        if (!task.parentTaskId.isEmpty() && taskWidgetsMap.contains(task.parentTaskId)) {
            // Це ДИТИНА. Шукаємо батька.
            TaskItemWidget *parentWidget = taskWidgetsMap[task.parentTaskId];
            parentWidget->addChildTask(item);
        } else {
            // Це КОРІНЬ (або сирота). Кладемо в категорію.
            if (categoryWidgetsMap.contains(task.categoryId)) {
                categoryWidgetsMap[task.categoryId]->addTaskWidget(item);
            }
        }
    }
}

void ToDoPage::onAddTaskClicked() {
    QString text = taskInput->text().trimmed();
    if (text.isEmpty()) return;

    ToDoModule *mod = getModule();
    if (mod) {
        QString catId = categoryCombo->currentData().toString();
        // Додаємо звичайне завдання (без батька)
        mod->addTask(text, catId, ""); 
        taskInput->clear();
        refreshData();
    }
}

void ToDoPage::onTaskStatusChanged(QString taskId, bool isDone) {
    ToDoModule *mod = getModule();
    if (mod) mod->toggleTask(taskId);
}

void ToDoPage::onAddCategoryClicked() {
    // Цей слот тепер не використовується в новій логіці,
    // але можна лишити для сумісності або видалити з хедеру.
}

ToDoModule* ToDoPage::getModule() {
    return todoModule;
}

void ToDoPage::clearLayout() {
    QLayoutItem *item;
    // Ми залишаємо останній елемент (> 1), тому що це "пружина" (addStretch),
    // яка тримає все зверху. Ми не хочемо її видаляти.
    while (categoriesLayout->count() > 1) {
        item = categoriesLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}