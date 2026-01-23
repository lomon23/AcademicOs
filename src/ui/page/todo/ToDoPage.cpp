#include "ToDoPage.h"
#include "TaskItemWidget.h"
#include <QRandomGenerator>
#include <QMap>

ToDoPage::ToDoPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ToDoPage::setupUI() {
    // 🔥 ГЛОБАЛЬНИЙ ЛЕЙАУТ - ГОРИЗОНТАЛЬНИЙ (Ліво | Право)
    QHBoxLayout *globalLayout = new QHBoxLayout(this);
    globalLayout->setContentsMargins(0, 0, 0, 0);
    globalLayout->setSpacing(0);

    // === ЛІВА ЧАСТИНА (ОСНОВНИЙ КОНТЕНТ) ===
    QWidget *mainContent = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 1. Header
    QLabel *title = new QLabel("Tasks", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);

    // 2. Inline Category Creation + COLOR PICKER
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(10);

    QLineEdit *newCatInput = new QLineEdit(this);
    newCatInput->setPlaceholderText("New List Name...");
    newCatInput->setFixedWidth(200);
    newCatInput->setStyleSheet("background: #2D2D2D; color: white; border: 1px solid #444; border-radius: 4px; padding: 6px;");
    topLayout->addWidget(newCatInput);

    // 🎨 Кнопка вибору кольору (Кружечок)
    colorSelectorBtn = new QPushButton(this);
    colorSelectorBtn->setFixedSize(28, 28);
    colorSelectorBtn->setCursor(Qt::PointingHandCursor);
    // Початковий колір
    colorSelectorBtn->setStyleSheet(QString("background-color: %1; border-radius: 14px; border: 2px solid #444;").arg(categoryColors[0]));
    
    connect(colorSelectorBtn, &QPushButton::clicked, this, &ToDoPage::cycleColor);
    topLayout->addWidget(colorSelectorBtn);

    // Кнопка Add
    QPushButton *addCatBtn = new QPushButton("Add List", this);
    addCatBtn->setCursor(Qt::PointingHandCursor);
    addCatBtn->setStyleSheet("background-color: #444; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold;");
    topLayout->addWidget(addCatBtn);

    // Логіка додавання з вибраним кольором
    connect(addCatBtn, &QPushButton::clicked, [this, newCatInput](){
        QString text = newCatInput->text().trimmed();
        if (!text.isEmpty()) {
            ToDoModule *mod = getModule();
            if (mod) {
                // Беремо поточний вибраний колір
                QString color = categoryColors[currentColorIndex];
                mod->addCategory(text, color);
                newCatInput->clear();
                refreshData();
            }
        }
    });

    topLayout->addStretch();
    mainLayout->addWidget(topBar);

    // 3. Task Input
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

    // 4. Scroll Area
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

    // Додаємо ліву частину в глобальний лейаут
    globalLayout->addWidget(mainContent, 1); // 1 = Stretch factor (займає всю доступну ширину)

    // === ПРАВА ЧАСТИНА (ANALYTICS) ===
    rightBar = new ToDoRightBar(this);
    globalLayout->addWidget(rightBar); // Додаємо справа
}

// Метод зміни кольору
void ToDoPage::cycleColor() {
    currentColorIndex = (currentColorIndex + 1) % categoryColors.size();
    QString color = categoryColors[currentColorIndex];
    colorSelectorBtn->setStyleSheet(QString("background-color: %1; border-radius: 14px; border: 2px solid #444;").arg(color));
}

void ToDoPage::refreshData() {
    ToDoModule *mod = getModule();
    if (!mod) return;

    // --- Оновлення Списку Тасків (Старий код) ---
    clearLayout();
    categoryCombo->clear();

    QVector<ToDoCategory> categories = mod->getCategories();
    QMap<QString, TaskItemWidget*> taskWidgetsMap;
    QMap<QString, CategoryWidget*> categoryWidgetsMap;

    for (const auto &cat : categories) {
        categoryCombo->addItem(cat.name, cat.id);
        CategoryWidget *catWidget = new CategoryWidget(cat, this);
        categoryWidgetsMap[cat.id] = catWidget;
        categoriesLayout->insertWidget(categoriesLayout->count() - 1, catWidget);
    }

    QVector<ToDoTask> allTasks;
    for (const auto &cat : categories) {
        allTasks.append(mod->getTasksByCategory(cat.id));
    }

    for (const auto &task : allTasks) {
        TaskItemWidget *item = new TaskItemWidget(task, this);
        connect(item, &TaskItemWidget::statusChanged, this, &ToDoPage::onTaskStatusChanged);
        connect(item, &TaskItemWidget::deleteRequested, [this, mod](QString id){
            mod->deleteTask(id);
            refreshData();
        });
        connect(item, &TaskItemWidget::renameRequested, [mod](QString id, QString newTitle){
            mod->renameTask(id, newTitle);
        });
        connect(item, &TaskItemWidget::addSubTaskRequested, [this, mod, task](QString parentId){
            mod->addTask("New Sub-task", task.categoryId, parentId);
            refreshData();
        });
        taskWidgetsMap[task.id] = item;
    }

    for (const auto &task : allTasks) {
        TaskItemWidget *item = taskWidgetsMap[task.id];
        if (!task.parentTaskId.isEmpty() && taskWidgetsMap.contains(task.parentTaskId)) {
            TaskItemWidget *parentWidget = taskWidgetsMap[task.parentTaskId];
            parentWidget->addChildTask(item);
        } else {
            if (categoryWidgetsMap.contains(task.categoryId)) {
                categoryWidgetsMap[task.categoryId]->addTaskWidget(item);
            }
        }
    }

    // --- 🔥 ОНОВЛЮЄМО СТАТИСТИКУ СПРАВА ---
    rightBar->refreshStats(mod);
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
    if (mod) {
        mod->toggleTask(taskId);
        
        // 👇 ДОДАЙ ЦЕЙ РЯДОК
        rightBar->refreshStats(mod); 
    }
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