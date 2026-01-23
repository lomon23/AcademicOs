#include "ToDoPage.h"
#include "TaskItemWidget.h"
#include "../../MainWindow.h" // Щоб доступитися до списку модулів (або через parent)
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

ToDoPage::ToDoPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ToDoPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // --- 1. ЗАГОЛОВОК ---
    QLabel *title = new QLabel("Tasks", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    mainLayout->addWidget(title);

    // --- 2. ЗОНА ДОДАВАННЯ (Input Area) ---
    QWidget *inputContainer = new QWidget(this);
    inputContainer->setStyleSheet("background-color: #2D2D2D; border-radius: 8px;");
    QHBoxLayout *inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(10, 10, 10, 10);

    // Поле вводу
    taskInput = new QLineEdit(this);
    taskInput->setPlaceholderText("What needs to be done?");
    taskInput->setStyleSheet("border: none; color: white; font-size: 16px; background: transparent;");
    inputLayout->addWidget(taskInput, 1);

    // Вибір категорії
    categoryCombo = new QComboBox(this);
    categoryCombo->setFixedWidth(150);
    categoryCombo->setStyleSheet("background: #1E1E1E; color: white; border: 1px solid #444; border-radius: 4px; padding: 4px;");
    inputLayout->addWidget(categoryCombo);

    // Кнопка "+"
    QPushButton *addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(32, 32);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background-color: #00E676; color: black; font-weight: bold; border-radius: 16px; border: none;");
    connect(addBtn, &QPushButton::clicked, this, &ToDoPage::onAddTaskClicked);
    // Також додаємо по Enter
    connect(taskInput, &QLineEdit::returnPressed, this, &ToDoPage::onAddTaskClicked);
    inputLayout->addWidget(addBtn);

    mainLayout->addWidget(inputContainer);

    // --- 3. СПИСОК (Scroll Area) ---
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;"); // Прозорий фон

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    categoriesLayout = new QVBoxLayout(scrollContent);
    categoriesLayout->setContentsMargins(0, 0, 0, 0);
    categoriesLayout->setSpacing(15);
    categoriesLayout->addStretch(); // Пружина знизу, щоб категорії підтискало вгору

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void ToDoPage::refreshData() {
    ToDoModule *mod = getModule();
    if (!mod) return;

    // 1. Очищаємо старе
    clearLayout();
    categoryCombo->clear();

    // 2. Отримуємо дані
    QVector<ToDoCategory> categories = mod->getCategories();
    
    // 3. Будуємо віджети
    // Важливий момент: ми вставляємо віджети ПЕРЕД пружиною (addStretch), 
    // тому використовуємо insertWidget(count-1) або просто видаляємо пружину і додаємо нову.
    // Найпростіше: видалити все і додати заново.
    
    for (const auto &cat : categories) {
        // Додаємо в комбобокс
        categoryCombo->addItem(cat.name, cat.id); // name - текст, id - data

        // Створюємо віджет категорії
        CategoryWidget *catWidget = new CategoryWidget(cat, this);
        
        // Знаходимо таски для цієї категорії
        QVector<ToDoTask> tasks = mod->getTasksByCategory(cat.id);
        for (const auto &task : tasks) {
            TaskItemWidget *item = new TaskItemWidget(task, catWidget);
            
            // З'єднуємо сигнал зміни статусу
            connect(item, &TaskItemWidget::statusChanged, this, &ToDoPage::onTaskStatusChanged);
            
            catWidget->addTaskWidget(item);
        }

        // Додаємо категорію в лейаут (перед розтяжкою)
        categoriesLayout->insertWidget(categoriesLayout->count() - 1, catWidget);
    }
}

void ToDoPage::onAddTaskClicked() {
    QString text = taskInput->text().trimmed();
    if (text.isEmpty()) return;

    ToDoModule *mod = getModule();
    if (mod) {
        QString catId = categoryCombo->currentData().toString();
        mod->addTask(text, catId);
        
        taskInput->clear();
        refreshData(); // Перемальовуємо список
    }
}

void ToDoPage::onTaskStatusChanged(QString taskId, bool isDone) {
    ToDoModule *mod = getModule();
    if (mod) {
        // Тут ми просто викликаємо toggle, бо модуль сам знає що робити. 
        // Або можна зробити setTaskStatus(id, status). 
        // Поки що у нас є toggleTask.
        mod->toggleTask(taskId);
        
        // Тут refreshData не обов'язковий, бо віджет вже змінив свій вигляд візуально.
        // Але якщо ми хочемо сортувати виконані вниз - тоді треба refresh.
        // Поки лишимо без рефрешу для плавності.
    }
}

ToDoModule* ToDoPage::getModule() { return todoModule; }

void ToDoPage::clearLayout() {
    QLayoutItem *item;
    // Видаляємо все крім останнього (stretch)
    while (categoriesLayout->count() > 1) {
        item = categoriesLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }
}