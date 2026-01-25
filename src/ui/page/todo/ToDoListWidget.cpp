#include "ToDoListWidget.h"
#include <QDebug>

ToDoListWidget::ToDoListWidget(ToDoModule* module, const ToDoCategory &category, QWidget *parent)
    : QWidget(parent), m_module(module), m_cat(category)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    this->setObjectName("ListContainer");
    // 1. ХЕДЕР (Тільки якщо це не "Всі" і не "Inbox" - хоча Inbox можна редагувати)
    // Для простоти додаємо хедер для всіх звичайних категорій
    setupHeader();

    // 2. Контейнер для тасок
    tasksLayout = new QVBoxLayout();
    tasksLayout->setSpacing(5);
    tasksLayout->setAlignment(Qt::AlignTop);
    mainLayout->addLayout(tasksLayout);

    // 3. Кнопка "Add Task" знизу (якщо треба, але ми юзаємо Inline)
    // ... (твоя стара логіка, якщо вона була, або просто розтяжка)
    mainLayout->addStretch();
}

void ToDoListWidget::setupHeader() {
    QHBoxLayout *header = new QHBoxLayout();
    header->setContentsMargins(10, 10, 10, 20); 

    // 1. COLOR CIRCLE
    colorBtn = new QPushButton(this);
    colorBtn->setFixedSize(24, 24);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setStyleSheet(QString(
        "background-color: %1; border: none; border-radius: 12px;"
    ).arg(m_cat.color));
    
    connect(colorBtn, &QPushButton::clicked, this, &ToDoListWidget::cycleColor);
    header->addWidget(colorBtn);

    // 2. TITLE INPUT
    titleEdit = new QLineEdit(m_cat.name, this);
    titleEdit->setStyleSheet(
        "QLineEdit { background: transparent; border: none; color: white; font-size: 24px; font-weight: bold; }"
        "QLineEdit:focus { border-bottom: 2px solid #555; }"
    );
    
    // 🔥 ФІКС ТУТ:
    
    // А) Зберігаємо в базу МИТТЄВО (щоб дані не пропали)
    connect(titleEdit, &QLineEdit::textEdited, [this](const QString &text){
        m_cat.name = text;
        m_module->updateCategory(m_cat.id, m_cat.name, m_cat.color);
        // АЛЕ НЕ ВИКЛИКАЄМО refreshData(), щоб не збити фокус
    });

    // Б) Оновлюємо Таби (зверху) тільки коли закінчили редагувати (Enter або клік вбік)
    connect(titleEdit, &QLineEdit::editingFinished, [this](){
        emit categoryChanged(); 
    });

    header->addWidget(titleEdit, 1);

    // 3. ADD BUTTON (+)
    QPushButton *addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(30, 30);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background: #333; color: white; border-radius: 4px; border: none; font-weight: bold; font-size: 18px;");
    addBtn->setToolTip("Add Task");

    connect(addBtn, &QPushButton::clicked, [this](){
        emit addTaskRequested(m_cat.id);
    });
    header->addWidget(addBtn);

    // 4. DELETE BUTTON
    QPushButton *delBtn = new QPushButton("🗑", this); 
    delBtn->setFixedSize(30, 30);
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setStyleSheet("background: transparent; color: #666; font-size: 16px; border: none;");
    delBtn->setToolTip("Delete List");
    
    connect(delBtn, &QPushButton::clicked, [this](){
        emit deleteCategoryRequested(m_cat.id);
    });
    
    header->addWidget(delBtn);

    mainLayout->addLayout(header);
}
void ToDoListWidget::addTaskWidget(QWidget *w) {
    tasksLayout->addWidget(w);
}

void ToDoListWidget::cycleColor() {
    // Простий цикл кольорів (можеш розширити)
    QStringList colors = {"#FF5555", "#50FA7B", "#F1FA8C", "#BD93F9", "#FF79C6", "#8BE9FD"};
    
    int idx = colors.indexOf(m_cat.color);
    if (idx == -1) idx = 0;
    else idx = (idx + 1) % colors.size();
    
    m_cat.color = colors[idx];
    
    // Оновлюємо UI
    colorBtn->setStyleSheet(QString(
        "background-color: %1; border: none; border-radius: 12px;"
    ).arg(m_cat.color));
    
    // Зберігаємо
    m_module->updateCategory(m_cat.id, m_cat.name, m_cat.color);
    emit categoryChanged();
}