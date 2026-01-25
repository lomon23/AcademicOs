#include "ToDoListWidget.h"

ToDoListWidget::ToDoListWidget(const ToDoCategory &cat, QWidget *parent)
    : QWidget(parent), m_cat(cat)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 5, 0, 10);
    mainLayout->setSpacing(0);

    // 1. HEADER
    QWidget *header = new QWidget(this);
    header->setFixedHeight(32);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(5, 0, 5, 0);
    
    // Toggle Arrow
    toggleBtn = new QPushButton("▼", this);
    toggleBtn->setFixedSize(24, 24);
    toggleBtn->setStyleSheet("background: transparent; color: #888; border: none;");
    connect(toggleBtn, &QPushButton::clicked, [this](){
        isExpanded = !isExpanded;
        tasksContainer->setVisible(isExpanded);
        toggleBtn->setText(isExpanded ? "▼" : "▶");
    });
    headerLayout->addWidget(toggleBtn);

    // Name
    QLabel *title = new QLabel(cat.name, this);
    title->setStyleSheet(QString("font-weight: bold; font-size: 14px; color: %1;").arg(cat.color));
    headerLayout->addWidget(title);

    // Add Button [+]
    addBtn = new QPushButton("+", this);
    addBtn->setFixedSize(24, 24);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet("background: transparent; color: #666; border: 1px solid #444; border-radius: 4px;");
    connect(addBtn, &QPushButton::clicked, [this](){ emit addTaskRequested(m_cat.id); });
    
    headerLayout->addStretch(); // Пружина
    headerLayout->addWidget(addBtn);

    mainLayout->addWidget(header);

    // 2. TASKS CONTAINER
    tasksContainer = new QWidget(this);
    tasksLayout = new QVBoxLayout(tasksContainer);
    tasksLayout->setContentsMargins(0, 0, 0, 0); // Без відступів тут, відступи будуть в тасках
    tasksLayout->setSpacing(2);
    tasksLayout->setAlignment(Qt::AlignTop);

    mainLayout->addWidget(tasksContainer);
}

void ToDoListWidget::addTaskWidget(TaskItemWidget *widget) {
    tasksLayout->addWidget(widget);
}

void ToDoListWidget::clear() {
    QLayoutItem *item;
    while ((item = tasksLayout->takeAt(0))) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}